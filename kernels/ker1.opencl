#pragma OPENCL EXTENSION cl_khr_fp16 : enable
#define GR_SIZE 256

#define SAMPLE_ITERS 20

float square(float3 A, float3 B, float3 C, float3 D) {
    return (length(cross(B - A, C - A)) + length(cross(C - A, D - A))) / 2;
}

float triangle_square(float3 A, float3 B, float3 C) {
    return length(cross(B - A, C - A)) / 2;
}

float DecodeShadow(float4 f) {
    return f.x;
}

float4 magicMult(float16 mat, float4 f) {
    return (float4)(mat.s0123 * f.x + mat.s4567 * f.y + mat.s89AB * f.z + mat.sCDEF * f.w);
}

float4 magicMultV2(float16 mat, float4 f) {
    float4 res = {dot(mat.s0123, f), dot(mat.s4567, f), dot(mat.s89AB, f), dot(mat.sCDEF, f)};
    return res;
}

float4 make_float4(float a, float b, float c, float d)
{
  float4 res;
  res.x = a;
  res.y = b;
  res.z = c;
  res.w = d;
  return res;
}

__kernel void ComputeEmission(
    __global float4* patchPoints,
    __constant float2* hammersley,
    __constant float16* glightMatrix,
    __constant float* lightParams,
    image2d_t shadowMap,
    __global float4* colours,
    __global float4* emission,
    __global float4* indirect,
    __global float4* normals)
{
    int i = get_global_id(0);
    float16 lightMatrix = *glightMatrix;
    float innerAngle = lightParams[0];
    float outerAngle = lightParams[1];
    float3 lightPosition = {lightParams[2], lightParams[3], lightParams[4]};
    float3 lightDirection = {lightParams[5], lightParams[6], lightParams[7]};

    float3 A = patchPoints[i * 4 + 0].xyz;
    float3 B = patchPoints[i * 4 + 1].xyz;
    float3 C = patchPoints[i * 4 + 3].xyz;

    if (dot(lightPosition - (B + C) / 2, normals[i].xyz) <= 0) {
        emission[i] = make_float4(0, 0, 0, 0);
        indirect[i] = make_float4(0, 0, 0, 0);
        return;
    }

    float resultEmission = 0;

    sampler_t sampler = CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_CLAMP | CLK_FILTER_LINEAR;

    float3 AB = (B - A);
    float3 AC = (C - A);
    for (int j = 0; j < SAMPLE_ITERS; ++j) {
        float3 point = A + AB * hammersley[j].x + AC * hammersley[j].y;
        float4 p4 = (float4){point.x, point.y, point.z, 1};
        float4 lightPoint = magicMultV2(lightMatrix, p4);
        float3 lightProj = lightPoint.xyz / lightPoint.w / 2 + (float3){0.5f, 0.5f, 0.5f};
        float depth = DecodeShadow(read_imagef(shadowMap, sampler, lightProj.xy));
        if (depth > lightProj.z - 0.00001f) {
            float currentAngle = dot(lightDirection, normalize(point - lightPosition));
            float angleImpact = clamp((outerAngle - currentAngle)
                                    / (outerAngle - innerAngle), 0.0f, 1.0f);
            resultEmission += angleImpact;
        }
    }
    resultEmission /= SAMPLE_ITERS;
    float3 vec_to_light = lightPosition - (B + C) / 2;
    float angle_influence = dot(vec_to_light, normals[i].xyz) / length(vec_to_light);
    emission[i] = resultEmission * colours[i] * angle_influence;
    indirect[i] = make_float4(0, 0, 0, 0);
}

__kernel void Radiosity(
    __global float4* excident,
    __global float4* ff,
    __global short4* ffIndices,
    __global uint* offsets,
    __global float4* colors,
    __global float4* incident)
{
    int i  = get_global_id(0);
    int start = offsets[i];
    int finish = offsets[i + 1];
    float4 result = {0.0f, 0.0f, 0.0f, 0.0f};
    for (int j = start; j < finish; j += 4) {
        float4 ff_value = ff[j / 4];
        short4 index = ffIndices[j / 4];
        result += ff_value.x * excident[index.x];
        result += ff_value.y * excident[index.y];
        result += ff_value.z * excident[index.z];
        result += ff_value.w * excident[index.w];
    }
    incident[i] = result;
}

__kernel void PrepareBuffers(
    __global float4* excident,
    __global float4* incident,
    __global float4* indirect,
    __global float4* color)
{
    int i = get_global_id(0);
    indirect[i] += incident[i];
    excident[i] = incident[i] * color[i];
}

__kernel void ComputeIndirect(
    __global float4* indices,
    __global float4* weights,
    __global float4* patchesIndirect,
    __global float4* pointsIndirect)
{
    int i = get_global_id(0);
    float4 result = {0.0f, 0.0f, 0.0f, 0.0f};
    float4 ind = indices[i];
    float4 wgh = weights[i];
    result += patchesIndirect[(int)ind.x] * wgh.x;
    result += patchesIndirect[(int)ind.y] * wgh.y;
    result += patchesIndirect[(int)ind.z] * wgh.z;
    result += patchesIndirect[(int)ind.w] * wgh.w;
    pointsIndirect[i] = result;
}

__kernel void Compress(
    __global float* fullArray,
    __global half* halfArray) {

    int i = get_global_id(0);
    vstore_half(fullArray[i], i, halfArray);
}
