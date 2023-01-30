matrix World;
matrix View;
matrix Projection;
uint Index;

struct VertexInput
{
    float4 Position : Position;
};

struct VertexOutput
{
    float4 Position : SV_Position;
};

VertexOutput VS(VertexInput input)
{
    // 위치를 어떤 공간에 곱하면 그 공간으로 이동한다고 생각하면 된다.
    VertexOutput output;
    output.Position = mul(input.Position, World); 
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    
    return output;
}

float4 PS_R(VertexOutput input) : SV_Target
{
    if(Index == 0)
        return float4(1, 0, 0, 1);
    if (Index == 1)
        return float4(0, 1, 0, 1);
    if (Index == 2)
        return float4(0, 0, 1, 1);

    return float4(0, 0, 0, 1);
}

// 선으로만 그리기
RasterizerState FillMode_Wireframe
{
    FillMode = Wireframe;
};

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS_R()));
    }

    pass P1
    {
        SetRasterizerState(FillMode_Wireframe);

        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS_R()));
    }
}