


void WallVP(
	float4 pos : POSITION,
	
	out float4 oPos : POSITION,
	out float oWallDistance : TEXCOORD0,
	
	uniform float4x4 modelViewProj,
	uniform float4x4 world,
	uniform float4 cameraPos
) {
	oPos = mul(pos, modelViewProj);
	
	
	float4 worldPos = mul(pos, world);
	oWallDistance = length(cameraPos - pos);
}

void WallFP(
	float wallDistance : TEXCOORD0,
	
	out float4 oColor : COLOR,
	
	uniform float maxWallDistance
) {
	float w = wallDistance / maxWallDistance;
	w = saturate(w);

	oColor = float4(1, 0, 0, w);
}