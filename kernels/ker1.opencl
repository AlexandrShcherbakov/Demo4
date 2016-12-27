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
    __global half* patchPoints,
    __constant half* hammersley,
    __constant float16* glightMatrix,
    __constant float* lightParams,
    image2d_t shadowMap,
    __global half* colours,
    __global half* emission,
    __global half* indirect,
    __global half* normals)
{
    int i = get_global_id(0);
    float16 lightMatrix = *glightMatrix;
    float innerRadius = lightParams[0];
    float outerRadius = lightParams[1];
    float3 lightPosition = {lightParams[2], lightParams[3], lightParams[4]};
    float3 lightDirection = {lightParams[5], lightParams[6], lightParams[7]};

    float3 A = vload_half4(i * 4 + 0, patchPoints).xyz;
    float3 B = vload_half4(i * 4 + 1, patchPoints).xyz;
    float3 C = vload_half4(i * 4 + 3, patchPoints).xyz;

    vstore_half4(make_float4(0, 0, 0, 0), i, indirect);

    if (dot(-lightDirection, vload_half4(i, normals).xyz) <= 0) {
        vstore_half4(make_float4(0, 0, 0, 0), i, emission);
        return;
    }

    float resultEmission = 0;

    sampler_t sampler = CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_CLAMP | CLK_FILTER_LINEAR;

    float3 AB = (B - A);
    float3 AC = (C - A);
    for (int j = 0; j < SAMPLE_ITERS; ++j) {
        float2 quadPoint = vload_half2(j, hammersley);
        float3 point = A + AB * quadPoint.x + AC * quadPoint.y;
        float4 p4 = (float4){point.x, point.y, point.z, 1};
        float4 lightPoint = magicMultV2(lightMatrix, p4);
        float3 lightProj = lightPoint.xyz / lightPoint.w / 2 + (float3){0.5f, 0.5f, 0.5f};
        float depth = DecodeShadow(read_imagef(shadowMap, sampler, lightProj.xy));
        if (depth > lightProj.z - 0.00001f) {
            float currentRadius = length(cross(lightDirection, point - lightPosition));
            float distantImpact = clamp((outerRadius - currentRadius)
                                    / (outerRadius - innerRadius), 0.0f, 1.0f);
            resultEmission += distantImpact;
        }
    }
    resultEmission /= SAMPLE_ITERS;
    float angle_influence = dot(-lightDirection, vload_half4(i, normals).xyz);
    vstore_half4(resultEmission * vload_half4(i, colours) * angle_influence, i, emission);
}

__kernel void Radiosity(
    __global half* excident,
    __global half* ff,
    __global short4* ffIndices,
    __global uint* offsets,
    __global half* incident)
{
    int i  = get_global_id(0);
    int start = offsets[i];
    int finish = offsets[i + 1];
    float4 result = {0.0f, 0.0f, 0.0f, 0.0f};
    for (int j = start; j < finish; j += 4) {
        float4 ff_value = vload_half4(j / 4, ff);
        short4 index = ffIndices[j / 4];
        result += ff_value.x * vload_half4(index.x, excident);
        result += ff_value.y * vload_half4(index.y, excident);
        result += ff_value.z * vload_half4(index.z, excident);
        result += ff_value.w * vload_half4(index.w, excident);
    }
    vstore_half4(result, i, incident);
}

__kernel void PrepareBuffers(
    __global half* excident,
    __global half* incident,
    __global half* indirect,
    __global half* color)
{
    int i = get_global_id(0);
    float4 inc = vload_half4(i, incident);
    vstore_half4(vload_half4(i, indirect) + inc, i, indirect);
    vstore_half4(inc * vload_half4(i, color), i, excident);
}

__kernel void ComputeIndirect(
    __global short4* indices,
    __global half* weights,
    __global half* patchesIndirect,
    __global float4* pointsIndirect)
{
    int i = get_global_id(0);
    float4 result = {0.0f, 0.0f, 0.0f, 0.0f};
    short4 ind = indices[i];
    float4 wgh = vload_half4(i, weights);
    result += vload_half4(ind.x, patchesIndirect) * wgh.x;
    result += vload_half4(ind.y, patchesIndirect) * wgh.y;
    result += vload_half4(ind.z, patchesIndirect) * wgh.z;
    result += vload_half4(ind.w, patchesIndirect) * wgh.w;
    pointsIndirect[i] = result;
}

__kernel void Compress(
    __global float* fullArray,
    __global half* halfArray) {

    int i = get_global_id(0);
    vstore_half(fullArray[i], i, halfArray);
}
