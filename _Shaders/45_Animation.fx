#include "00_Global.fx"

float3 Direction = float3(-1, -1, +1);

struct VertexModel
{
    float4 Position : Position;
    float2 Uv : Uv;
    float3 Normal : Normal;
    float3 Tangent : Tangent;
    float4 BlendIndices : BlendIndices;
    float4 BlendWeights : BlendWeights;
};

#define MAX_MODEL_TRANSFORMS 250
#define MAX_MODEL_KEYFRAMES 500

cbuffer CB_Bone
{
    matrix BoneTransforms[MAX_MODEL_TRANSFORMS];
    
    uint BoneIndex;
};

struct AnimationFrame
{
    int Clip;

    uint CurrFrame;
    uint NextFrame;

    float Time;
    float Running;

    float3 Padding;
};

cbuffer CB_TweenFrame
{
    AnimationFrame Keyframes; // 구조체로 묶어서 전역변수로 사용하기 위함
};

Texture2DArray TransformsMap; // Shader SRV

struct VertexOutput
{
    float4 Position : SV_Position;
    float3 Normal : Normal;
    float2 Uv : Uv;
};

// inout : 파라미터를 입력받아서 출력용으로 보낸다는 의미
void SetAnimationWorld(inout matrix world, VertexModel input)
{
    float indices[4] = { input.BlendIndices.x, input.BlendIndices.y, input.BlendIndices.z, input.BlendIndices.w };
    float weights[4] = { input.BlendWeights.x, input.BlendWeights.y, input.BlendWeights.z, input.BlendWeights.w };
    
    int clip;
    int currFrame;
    int nextFrame;
    float time;

    clip = Keyframes.Clip;
    currFrame = Keyframes.CurrFrame;
    nextFrame = Keyframes.NextFrame;
    time = Keyframes.Time;
    
    float4 c0, c1, c2, c3;
    float4 n0, n1, n2, n3;
    
    matrix curr = 0;
    matrix transform = 0;
    
    [unroll(4)]
    for (int i = 0; i < 4; i++)
    {
        // X값-열 (indices[i] * 4 + 0) : 4픽셀로 나눠서 들어온 Bone 번호
        // Y값-행 (currFrame) : 키 프레임
        // Z값-면 (clip) : 애니메이션 클립
        c0 = TransformsMap.Load(int4(indices[i] * 4 + 0, currFrame, clip, 0));
        c1 = TransformsMap.Load(int4(indices[i] * 4 + 1, currFrame, clip, 0));
        c2 = TransformsMap.Load(int4(indices[i] * 4 + 2, currFrame, clip, 0));
        c3 = TransformsMap.Load(int4(indices[i] * 4 + 3, currFrame, clip, 0));
        curr = matrix(c0, c1, c2, c3);

        // weights행렬에 가중치를 누적시켜서 곱하기 => 최종행렬 transform 도출
        transform += mul(weights[i], curr); 
    }

    // transform에 World를 구하여 World 상태 행렬 구하기
    // transform: 애니메이션이 이동할 행렬 / world: 모델(애니메이션)이 출력될 행렬
    // World는 Bone 번호와 키프레임, 클립을 전부 곱한 가중치가 전부 결합되어 만들어진 최종 행렬이 된다.
    world = mul(transform, world);
}

VertexOutput VS(VertexModel input)
{
    VertexOutput output;
    
    //World = mul(BoneTransforms[BoneIndex], World);
    SetAnimationWorld(World, input); // 위에 소스코드 대체
    
    output.Position = WorldPosition(input.Position);
    output.Position = ViewProjection(output.Position);
    
    output.Normal = WorldNormal(input.Normal);
    output.Uv = input.Uv;
    
    return output;
}

float4 PS(VertexOutput input) : SV_Target
{
    float NdotL = dot(normalize(input.Normal), -Direction);
    
    return DiffuseMap.Sample(LinearSampler, input.Uv) * NdotL;
}

technique11 T0
{
    P_VP(P0, VS, PS)
    P_RS_VP(P1, FillMode_WireFrame, VS, PS)
}