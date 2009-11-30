void WallVP(
	float4 pos : POSITION,
	float3 normal : NORMAL,
	float2 uv : TEXCOORD0,
	
	out float4 oPos : POSITION,
	out float2 oUv : TEXCOORD0,
	out float oWallDistance : TEXCOORD1,
	out float oEdge : TEXCOORD2,
	
	uniform float4x4 worldViewProj,
	uniform float4x4 world,
	uniform float4 cameraPos
) {
	oPos = mul(worldViewProj, pos);
	oUv = uv;
		
	float4 worldPos = mul(world, pos);
	
	// Get distance from the camera to the vertex
	oWallDistance = oPos.z;
	
	float3 N = normalize(normal);
	float3 E = normalize(cameraPos - pos.xyz);
	oEdge = max(dot(N, E), 0.0f);
}

void WallFP(
	float2 uv : TEXCOORD0,
	float wallDistance : TEXCOORD1,
	float edge : TEXCOORD2,
	
	out float4 oColor : COLOR,
	
	//uniform float maxWallDistance,
	//uniform float minAlpha,
	
	uniform sampler2D diffuse,
	uniform sampler1D edgeRamp
) {
	const float maxWallDistance = 2.5f;
	const float minAlpha = 0.2f;

	float3 c = tex2D(diffuse, uv).rgb;

	// Fragments closer to the camera are more transparent
	float a = wallDistance / maxWallDistance;
	a = smoothstep(minAlpha, 1.0f, a);
	
	edge = tex1D(edgeRamp, edge).x;

	oColor = float4(c, a);
}

void CellVP(
	float4 pos : POSITION,
	float3 normal : NORMAL,
	float2 uv : TEXCOORD0,
	
	out float4 oPos : POSITION,
	out float2 oUv : TEXCOORD0,
	out float oEdge : TEXCOORD1,
	
	uniform float4x4 worldViewProj,
	uniform float4 cameraPos
) {
	oPos = mul(worldViewProj, pos);
	oUv = uv;
	
	float3 N = normalize(normal);
	float3 E = normalize(cameraPos - pos.xyz);
	oEdge = max(dot(N, E), 0.0f);
}

void CellFP(
	float2 uv : TEXCOORD0,
	float edge : TEXCOORD1,
	
	out float4 oColor : COLOR,
	
	//uniform float maxWallDistance,
	//uniform float minAlpha,
	
	uniform sampler2D diffuse,
	uniform sampler1D edgeRamp
) {
	float3 c = tex2D(diffuse, uv).rgb;
	edge = tex1D(edgeRamp, edge).x;

	oColor = float4(c, 1.0f);
}