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

float4 fp_main(float2 texCoord: TEXCOORD0) : COLOR {
	
	float4 c = tex2D(Blur0, texCoord);

	c = c * float4(0.9f, 0.85f, 0.75f, 1.0f);
	c = c - float4(0.2f, 0.2f, 0.2f, 0.0f);

	return c;
}