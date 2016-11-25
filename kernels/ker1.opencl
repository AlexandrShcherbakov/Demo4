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
    __global float4* emission)
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
    float len = length(AB);
    resultEmission *= len * len * get_global_size(0) / 2;
    emission[i] = resultEmission * colours[i];
}

__kernel void ComputeModalEmission(
    __global uint* indices,
    __global float4* patchPoints,
    __constant float2* bar_coords,
    __constant float16* glightMatrix,
    __constant float* lightParams,
    image2d_t shadowMap,
    __global float* emission)
{
    int i = get_global_id(0);
    float16 lightMatrix = *glightMatrix;
    float innerAngle = lightParams[0];
    float outerAngle = lightParams[1];
    float3 lightPosition = {lightParams[2], lightParams[3], lightParams[4]};
    float3 lightDirection = {lightParams[5], lightParams[6], lightParams[7]};

    float3 A = patchPoints[indices[i * 3 + 0]].xyz;
    float3 B = patchPoints[indices[i * 3 + 1]].xyz;
    float3 C = patchPoints[indices[i * 3 + 2]].xyz;

    float resultEmission = 0;

    sampler_t sampler = CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_CLAMP | CLK_FILTER_LINEAR;

    float3 AB = (B - A);
    float3 AC = (C - A);
    for (int j = 0; j < SAMPLE_ITERS; ++j) {
        float3 point = A + AB * bar_coords[j].x + AC * bar_coords[j].y;
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
    resultEmission *= triangle_square(A, B, C) * get_global_size(0) * 40;
    emission[i] = resultEmission;
}

__kernel void ComputePatchEmission(
    __global float* modelEmission,
    __global uint* relIndices,
    __global float* relWeights,
    __global uint* offsets,
    __global float4* colours,
    __global float4* resultEmission)
{
    int i = get_global_id(0);
    float result = 0;
    int start = offsets[i];
    int finish = offsets[i + 1];

    for (int j = start; j < finish; ++j) {
        result += modelEmission[relIndices[j]] * relWeights[j];
    }

    resultEmission[i] = result * colours[i];
}

__kernel void Radiosity(
    __global float4* excident,
    __global float* ff,
    __global uint* ffIndices,
    __global uint* offsets,
    __global float4* colors,
    __global float4* incident)
{
    int i  = get_global_id(0);
    int start = offsets[i];
    int finish = offsets[i + 1];
    float4 result = {0.0f, 0.0f, 0.0f, 0.0f};
    for (int j = start; j < finish; ++j) {
        result += ff[j] * excident[ffIndices[j]];
    }
    incident[i] = colors[i] * result;
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
    if (ind.x == 0) {
        pointsIndirect[i] = make_float4(0, 1, 0, 0);
    } else {
        result += patchesIndirect[(int)ind.x] * wgh.x;
        result += patchesIndirect[(int)ind.y] * wgh.y;
        result += patchesIndirect[(int)ind.z] * wgh.z;
        result += patchesIndirect[(int)ind.w] * wgh.w;
        pointsIndirect[i] = result;
    }
    //pointsIndirect[i] = patchesIndirect[100];
}

__kernel void Compress(
    __global float* fullArray,
    __global half* halfArray) {

    int i = get_global_id(0);
    vstore_half(fullArray[i], i, halfArray);
}
