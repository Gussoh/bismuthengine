void WallVP(
	float4 pos : POSITION,
	float2 uv : TEXCOORD0,
	
	out float4 oPos : POSITION,
	out float oUv : TEXCOORD0,
	out float oWallDistance : TEXCOORD1,
	
	uniform float4x4 modelViewProj,
	uniform float4x4 world,
	uniform float4 cameraPos
) {
	oPos = mul(pos, modelViewProj);
	oUv = uv;
		
	float4 worldPos = mul(pos, world);
	
	// Get distance from the camera to the vertex
	oWallDistance = length(cameraPos - pos);
}

void WallFP(
	float uv : TEXCOORD0,
	float wallDistance : TEXCOORD1,
	
	out float4 oColor : COLOR,
	
	uniform float maxWallDistance,
	uniform float minAlpha,
	
	uniform sampler2D diffuse : sampler(s0)
) {
	float3 c = tex2D(diffuse, uv);

	// Fragments closer to the camera are more transparent
	float a = wallDistance / maxWallDistance;
	a = saturate(minAlpha + a);

	oColor = float4(c, a);
}