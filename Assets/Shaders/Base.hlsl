void WallVP(
	float4 pos : POSITION,
	float2 uv : TEXCOORD0,
	
	out float4 oPos : POSITION,
	out float2 oUv : TEXCOORD0,
	out float oWallDistance : TEXCOORD1,
	
	uniform float4x4 worldViewProj,
	uniform float4x4 world,
	uniform float4 cameraPos
) {
	oPos = mul(worldViewProj, pos);
	oUv = uv;
		
	float4 worldPos = mul(world, pos);
	
	// Get distance from the camera to the vertex
	oWallDistance = length(cameraPos - worldPos);
}

void WallFP(
	float2 uv : TEXCOORD0,
	float wallDistance : TEXCOORD1,
	
	out float4 oColor : COLOR,
	
	//uniform float maxWallDistance,
	//uniform float minAlpha,
	
	uniform sampler2D diffuse
) {
	const float maxWallDistance = 10.0f;
	const float minAlpha = 0.0f;

	float3 c = tex2D(diffuse, uv).rgb;

	// Fragments closer to the camera are more transparent
	float a = wallDistance / maxWallDistance;
	a = saturate(minAlpha + a);

	oColor = float4(c, a);
}