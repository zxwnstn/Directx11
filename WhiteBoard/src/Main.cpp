#include <windows.h>
#include <iostream>
#include <thread>
#include <string>

int main()
{
	std::string path = "E:\\dev\\Directx11\\Engine\\assets\\Scene\\sefs.scene";

	auto slash = path.rfind('\\');
	auto extension = path.rfind('.');
	auto saveSceneName = path.substr(slash + 1, extension - slash - 1);

	
}

/*
안녕하세요 Directx11을 활용해 구현해본 렌더링 엔진 소개영상입니다.

제가 구현한 기능 목록입니다.

1. fbx및 obj파일로 부터 모델정보 추출및 3D애니메이션 재생

2. Blinn-phong 쉐이딩 기반의 forward, deffered 렌더링

3. Shadow Mapping

4. Tesselation(Curved Pn algorithm)

5. HDR 렌더링

6. 모델, 카메라, 광원 객체에 대한 간단한 조작ui구현

이에 대해 하나하나 살펴보도록 하겠습니다.

이중 6번 항목은 ImGui라이브러리를 사용하여 구현하였으며, 세부설명은 생략하도록 하겟습니다.



===== 1. fbx및 obj파일로 부터 모델정보 추출및 3D애니메이션


먼저 obj파일의 경우 파싱하기 쉬운 자료구조로, 외부라이브러리를 이용하지않고 직접 파싱알고리즘을 작성했습니다.

메쉬, material, texture등의 데이터를 추출합니다.


fbx파일의 정보추출에는 fbxsdk를 활용했습니다.

메쉬, material, texture, bone, animation key frame 정보등을 추출합니다.

fbx의 경우 sdk를 사용하여 파싱하는데에 어느정도의 시간이 소요됩니다.

때문에 한번 파싱된 데이터들은 따로 파일로 캐싱하여 로딩시간을 단축시켰습니다.

캐싱에는 boost의 archive와 serialization을 활용했습니다.

모델정보 및 모든 asset들은 프로그램 초기화시 전부 메모리로 로드시키도록 설계했습니다.

때문에 씬에 모델을 추가할때는 파일 다일로그를 통해 파일을 지정하여 불러오는 방식이 아닌

미리 로드된 메쉬의 목록중에서 선택하도록 하였습니다.

메쉬타입은 Static, Skeletal로 분류되며 Fbx는 대부분 Skeletal, Obj는 전부 Static으로 지정됩니다.



Fbx의 Animation정보는 패널의 model탭에 보여집니다.

애니메이션의 속도, 반복등을 설정할수 있게 구성했습니다.


애니메이션 재생 구현의 설명입니다.

1. 각 bone마다 재생 시간에 대하여 앞 뒤 KeyFrame을 구형 또는 선형보간합니다.(Rotation은 쿼터니언 구형보간)

2. 보간된 KeyFrame matrix와 offset matrix로 SkinnedTransform을 얻어내고 쉐이더(주로 버텍스쉐이더)에 상수버퍼로 전달합니다.

3. 버텍스 쉐이더에서는 각 버텍스가 갖는 bone weight정보와 상수버퍼의 SkinnedTransform을 활용해 최종적인 SkinnedTransform을 계산합니다.



===== 2. Blinn-phong 쉐이딩 기반의 forward, deffered 렌더링

제가 적용한 라이팅 모델은 블린퐁 쉐이딩으로 가장 일반적인 방식을 사용했습니다.

Diffuse factor와 specular factor는 다음과 같은 알고리즘으로 구했습니다.

final color는 다음과 같이 구했습니다.



먼저 forward 렌더링부터 살펴보도록 하겟습니다. 제가 설정한 forward렌더링의 전체적인 렌더링패스는 다음과 같습니다.

1. 각 광원에 대한 Shadow Map 생성합니다.

2. 렌더타겟을 BackBuffer가 아닌 BackBuffer와 동일한 크기의 ForwardBuffer(임의명명)으로 설정합니다. ForwardBuffer의 내용은 초기화합니다.

3. 한 광원에 대해 모든 모델들을 렌더링합니다. ForwardBuffer의 내용을 BackBuffer에 Blending하여 렌더링합니다.

4. 다음 광원에 대해 2~3의 과정을 반복합니다. 입력된 모든 광원을 처리합니다.


이러한 렌더링 패스구성때문에 L * M 시간복잡도를 갖게됩니다.(L-광원의수, M-모델의 수)

다음은 deffered 렌더링을 살펴보도록 하겠습니다. 제가 설정한 deffered렌더링의 전체적인 렌더링패스는 다음과 같습니다.

1. 각 광원에 대한 Shadow Map 생성합니다.

2. 렌더타겟을 GBuffer로 설정합니다. GBuffer는 다섯개의 렌더타겟으로 구성했으며, 다이렉트x10이상부터 지원되는 멀티렌더 타겟을 활용합니다.

제가 구성한 GBuffer의 요소는 다음과 같습니다.

1. Diffuse 2. Normal 3. Ambient 4. WorldPosition 5. Specular,shiness

렌더링에 굳이 압축알고리즘은 적용하진 않았습니다.

3. 각 모델들에 대해 GBuffer 렌더링을 합니다.

4. 렌더타겟을 BackBuffer로 설정하고, 각 광원들에 대해 라이팅을 진행합니다.

이러한 렌더링 패스구성때문에 L + M 시간복잡도를 갖게됩니다.(L-광원의수, M-모델의 수)


3. Shadow Mapping 

제가 구현한 광원은 Spot Lihgt, Point Light, Directional Light로 총 3가지입니다.

각각의 광원마다 조도및 거리감쇄 알고리즘과 Shadow mapping하는 방법이 다릅니다.



3-1. Shadow map생성방식

3가지 방식모두 깊이값만을 기록하도록 구현했습니다.

Spot Light는 광원의 위치와 바라보는 방향으로 light ViewProjection matrix를 구성해 입력된 모델들을 렌더링합니다.


Point Light는 광원을 중심으로 앞뒤 상하좌우 6방면으로 light ViewProjection matrix들을 구성해 입력된 모델들을 렌더링합니다.

이때 각각의 ViewProjection matrix에 대해 모두 렌더링해야되므로 Geometry shader를 활용하고 텍스쳐는 TextureCubeMap형식을 사용합니다.


Directional Light는 케스케이드 쉐도우맵을 적용했습니다.

먼저 카메라 뷰프러스텀 중앙위치와 광원의 방향으로 ViewProjection matrix를 구합니다. 

하지만 이렇게만 하면 쉐도우맵의 디테일이 너무나 떨어지기 때문에 카메라로부터 거리에 따라 각기다른 Scale과 Translate을 적용한 3개의 ViewProjection matrix를 만들어냅니다.

이때 Scale은 카메라로부터 가까울수록 값이 커지기 때문에 카메라에 가까울수록 고해상도의 쉐도우맵을 얻게됩니다.

Point Light와 마찬가지로 여러개의 ViewProjection matrix가 있기때문에 Geometry shader를 활용하고 텍스쳐는 Texture2DArray형식을 사용합니다.


3-2. Shadow mapping방식

SpotLight와 Directional Light는 자신이 만들어진 ViewProjection matrix를 통해 해당 버텍스의 쉐도우맵 uv좌표를 구하고 깊이값을 비교합니다. 

PointLight의 경우 버텍스의 위치에서 PointLight로의 벡터로 쉐도우맵을 샘플합니다.

만약 쉐도우맵의 깊이값이 더 작다면 앞에 다른 object가 있다는 것이므로 그림자가 됩니다. 

부드러운 그림자를 위해 Directx에서 제공하는 하드웨어 PCF를 사용하였습니다.



4. Tesselation(Curved Pn Triangles algorithm)

자세한 작동 원리까지는 이해하지 못하였지만, Sample코드중 Curved Pn Tesselation을 참조하여 엔진에 이식해본 기능입니다.

삼각형의 vertex normal을 보간하여 곡선을 만들어내는 방식으로 만약 face normal을 사용하는 메쉬모델이라면 적용되지 않습니다. 

테셀레이션을 사용하므로 shader는 Vertex->Hull->Domain->Pixel쉐이더 순으로 구성하였습니다.

Shadow 맵의 경우 테셀레이션 된 메쉬로 렌더링하지 않기때문에 로우폴리곤 모델의 경우 그림자와 큰 차가 날수도 잇습니다.

이를 해결하기 위한 방법으로 Shadow맵을 생성할 당시 부터 모델에 테셀레이션을 적용하고 

Geometry 쉐이더로 테셀레이션된 메쉬를 가져와(Stream output기능을 활용) 재사용하는 것이 있습니다.(엔진내에 구현하지는 않았습니다.)



5. HDR 렌더링

deffered rendering 패스에 평균휘도 구하기와 톤매핑 패스를 추가하여 HDR렌더링을 구현하였습니다.

평균휘도를 구하는 것은 cpu에서만 해도 상관없고 gpu에서 모두 처리해도 상관없지만 

cpu에서 모두처리 하는것은 총 연산횟수가 약 백만번정도로 비교적 느리고 gpu에서 모두 처리하는 알고리즘은 컴퓨트쉐이더를 여러번 써야되서 비효율적이라 생각했습니다.

저는 절충안으로 gpu에서 1차 처리하고 cpu에서 나머지를 계산하는식으로 구현했습니다.  

gpu에서 1차처리가 이루어지면 cpu에서 처리하는 연산횟수가 만번이하로 크게 떨어집니다.

gpu에서 모두 처리하는 알고리즘에 비해 괄목할만한 성능향상은 이루어지지는 않습니다.


tone mapping방식으로는 Middle gray와 White값을 매개변수로 하는 ReinHard방식으로 구현했습니다.

HDR렌더링시 Gamma correction을 적용시켜야 제대로된 결과를 얻을수 있기때문에 

Diffuse맵 색상에 2.2승을(이미 감마보정이되있는 텍스쳐의 색상을 선형공간으로 되돌림) 취하고

라이팅연산이 끝난 최종컬러에 0.4545승을 취했습니다(다시 감마보정시킨다)

6. 결과물들

- material 조작화면(금속성)
- 다중 라이팅
- scene 저장 불러오기


*/