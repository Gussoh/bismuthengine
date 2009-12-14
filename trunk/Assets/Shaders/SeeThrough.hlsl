struct VS_OUTPUT {
   float4 Pos: POSITION;
   float2 texCoord: TEXCOORD0;
};

VS_OUTPUT vp_main(float4 Pos: POSITION){
   VS_OUTPUT Out;

   // Clean up inaccuracies
   Pos.xy = sign(Pos.xy);

   Out.Pos = float4(Pos.xy, 0, 1);
   // Image-space
   Out.texCoord.x = 0.5 * (1 + Pos.x);
   Out.texCoord.y = 0.5 * (1 - Pos.y);

   return Out;
}

sampler Blur0: register(s0);
sampler3D noise : register(s1);
uniform float time;

float4 fp_main(float2 texCoord: TEXCOORD0) : COLOR {
	
	float4 c = tex2D(Blur0, texCoord);
	c += 0.2f * (tex3D(noise, float3(texCoord * 5, time))) - 0.05f;
	
	return c;
}