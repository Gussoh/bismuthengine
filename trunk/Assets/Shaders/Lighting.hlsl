/** Expands a vector from [0, 1] to [-1, 1] **/
float3 expand(float3 v) {
	return (v - 0.5f) * 2.0f;
}

float cap(float v) {
#ifdef HDR
	return v;
#else
	return saturate(v);
#endif
}

/** Performs phong lighting **/
float4 phong(
	float3 normal, float3 lightDir, float3 halfAngle,
	float4 lightColour, float4 specularColour, float specularPower, 
	float4 ambientColour, float4 diffuseColour, float shadowAmount
	) {
	float nDotL = max(dot(normal, lightDir), 0.0f);
	float nDotH = saturate(dot(normal, halfAngle));
	
	float3 specular = specularColour.rgb * pow(nDotH, specularPower);
	
	return float4(((ambientColour.rgb + (nDotL * lightColour.rgb * shadowAmount)) * diffuseColour.rgb) + (specular * shadowAmount), diffuseColour.a);
}

/** Performs oren nayar lighting **/
/** Using a lookup texture will make it all fit nicely on ps_2_0 **/
float4 orenNayar(
	float3 normal, float3 lightDir, float3 viewDir,
	float4 lightColour, float roughness, 
	float4 ambientColour, float4 diffuseColour
#ifdef OREN_NAYAR_USE_LOOKUP
	, sampler2D lookupSampler
#endif
	) {
	float vDotN = dot(viewDir, normal);
	float lDotN = dot(lightDir, normal);
	float nDotL = cap(dot(normal, lightDir));

	float gamma = dot(viewDir - normal * vDotN, lightDir - lDotN);
	
	float rough_sq = roughness * roughness;
	
	float A = 1.0f - 0.5f * (rough_sq / (rough_sq + 0.57f));
	float B = 0.45f * (rough_sq / (rough_sq + 0.09f));
	
#ifndef OREN_NAYAR_USE_LOOKUP
	float alpha = max(acos(vDotN), acos(lDotN));
	float beta  = min(acos(vDotN), acos(lDotN)); 
	
	float C = sin(alpha) * tan(beta);
#else
	float texCoord = float2((vDotN + 1.0f) / 2.0f, (lDotN + 1.0f) / 2.0f);
	float C = tex2D(lookupSampler, texCoord);
#endif
	
	float3 final = (A + B * max(0.0f, gamma) * C);
	final = ambientColour.rgb + lightColour * nDotL * final;
	
	return float4(final * diffuseColour.rgb, diffuseColour.a);
}

/*** SHADOWS ***/

float2 vsmSample(sampler2D map, float2 uv) {
	float2 o = 1.0f / 1024.0f;
    float2 c = tex2D(map, uv.xy).rg; // center
    c += tex2D(map, uv.xy - o.xy).rg; // top left
    c += tex2D(map, uv.xy + o.xy).rg; // bottom right
    c += tex2D(map, float2(uv.x - o.x, uv.y)).rg; // left
    c += tex2D(map, float2(uv.x + o.x, uv.y)).rg; // right
    c += tex2D(map, float2(uv.x, uv.y + o.y)).rg; // bottom
    c += tex2D(map, float2(uv.x, uv.y - o.y)).rg; // top
    c += tex2D(map, float2(uv.x - o.x, uv.y + o.y)).rg; // bottom left
    c += tex2D(map, float2(uv.x + o.x, uv.y - o.y)).rg; // top right
    return c / 9;
}

float vsm(sampler2D shadowMap, float4 shadowUv, float distance) {
	float2 moments = tex2D(shadowMap, shadowUv.xy / shadowUv.w);
	
	float p = (distance <= moments.x);
	float variance = moments.y - (moments.x * moments.x);
	variance = min(max(variance, 0.000001f), 1.0f);
	float d = distance - moments.x;
	float p_max = variance / (variance + (d * d));
	
	return max(p, p_max);
}

float pcfSample(sampler2D shadowMap, float4 shadowUv, float distance, float texelSize) {
	float2 texCoord = shadowUv.xy / shadowUv.w;
	float3 offset = float3(texelSize, texelSize, -texelSize) * 0.33f;

	float shadow = distance <= tex2D(shadowMap, texCoord).x;
	shadow += distance <= tex2D(shadowMap, texCoord - offset.xy).x;
	shadow += distance <= tex2D(shadowMap, texCoord + offset.xy).x;
	shadow += distance <= tex2D(shadowMap, texCoord - offset.zy).x;
	shadow += distance <= tex2D(shadowMap, texCoord + offset.zy).x;
	shadow += distance <= tex2D(shadowMap, texCoord - offset.xy * 2.0f).x;
	shadow += distance <= tex2D(shadowMap, texCoord + offset.xy * 2.0f).x;
	shadow += distance <= tex2D(shadowMap, texCoord - offset.zy * 2.0f).x;
	shadow += distance <= tex2D(shadowMap, texCoord + offset.zy * 2.0f).x;
	shadow += distance <= tex2D(shadowMap, texCoord - offset.xy * 3.0f).x;
	shadow += distance <= tex2D(shadowMap, texCoord + offset.xy * 3.0f).x;
	shadow += distance <= tex2D(shadowMap, texCoord - offset.zy * 3.0f).x;
	shadow += distance <= tex2D(shadowMap, texCoord + offset.zy * 3.0f).x;
	
	shadow = shadow / 13.0f;
	return shadow;
}

float softSelfShadow(
	sampler2D shadowMap, float4 shadowUv, float texelSize, 
	float gradientClamp, float gradientScaleBias, float fixedDepthBias
	) {
	shadowUv = shadowUv / shadowUv.w;
	float centerDepth = tex2D(shadowMap, shadowUv.xy).x;
	
	float offset = texelSize * 0.4f;
	float4 depths = float4(
		tex2D(shadowMap, shadowUv.xy + float2(-offset, 0)).x,
		tex2D(shadowMap, shadowUv.xy + float2(offset, 0)).x,
		tex2D(shadowMap, shadowUv.xy + float2(0, -offset)).x,
		tex2D(shadowMap, shadowUv.xy + float2(0, offset)).x
		);
		
	float2 diff = abs(depths.yw - depths.xz);
	float gradient = min(gradientClamp, max(diff.x, diff.y));
	float gradientFactor = gradient * gradientScaleBias;
    
	float depthAdjust = gradientFactor + (fixedDepthBias * centerDepth);
	float finalCenterDepth = centerDepth + depthAdjust;

	depths += depthAdjust.xxxx;

	float final = finalCenterDepth > shadowUv.z;
	final += depths.x > shadowUv.z;
	final += depths.y > shadowUv.z;
	final += depths.z > shadowUv.z;
	final += depths.w > shadowUv.z;
	
	final += tex2D(shadowMap, shadowUv.xy + float2(-offset*2.0, 0)).x > shadowUv.z;
	final += tex2D(shadowMap, shadowUv.xy + float2(offset*2.0f, 0)).x > shadowUv.z;
	final += tex2D(shadowMap, shadowUv.xy + float2(0, -offset*2.0f)).x > shadowUv.z;
	final += tex2D(shadowMap, shadowUv.xy + float2(0, offset*2.0f)).x > shadowUv.z;
	
	final /= 9.0f;
	
	return final;
}

float shadow(
	sampler2D shadowMap1, float4 shadowUv1
	) {
	float distance = shadowUv1.z / shadowUv1.w;
	
    float shadow = 1;
    if (shadowUv1.z < 50.0f) {
        //shadow = vsm(shadowMap1, shadowUv1, distance);
		//shadow = distance <= (tex2D(shadowMap1, shadowUv1.xy / shadowUv1.w).x );
		//shadow = softSelfShadow(shadowMap1, shadowUv1, 1.0f / 2048.0f, 0.004f, 0.008f, 0.0021f);
		shadow = pcfSample(shadowMap1, shadowUv1, distance, 1.0f / 2048.0f);
    }
	
	return shadow;
}