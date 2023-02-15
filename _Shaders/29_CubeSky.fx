matrix World;
matrix View;
matrix Projection;

TextureCube SkyCubeMap;

struct VertexInput
{
    float4 Position : Position;
    float2 Uv : Uv;
    float3 Normal : Normal;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float3 oPosition : Position1; // Original Position
    float2 Uv : Uv;
    float3 Normal : Normal;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;

    // ���⺤�ͷ� ���� (w = 0)
    output.oPosition = input.Position.xyz;

    output.Position = mul(input.Position, World); 
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = mul(input.Normal, (float3x3) World);

    output.Uv = input.Uv;
    
    return output;
}

SamplerState LinearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

// ������ �ݽð����(CounterClock)����
RasterizerState FrontCounterClockWise_True
{
    FrontCounterClockWise = True;
};

// Rendering ���� ���ֱ�
DepthStencilState DepthEnable_False
{
    DepthEnable = false;
};

float4 PS(VertexOutput input) : SV_Target
{
    return SkyCubeMap.Sample(LinearSampler, input.oPosition);
}

technique11 T0
{
    pass P0
    {
        SetRasterizerState(FrontCounterClockWise_True);
        SetDepthStencilState(DepthEnable_False, 0);

        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}