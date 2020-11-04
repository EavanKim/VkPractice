(중요)
	기본적으로 x86 상태에서 프로그래밍 되어있습니다.
	용량제한을 회피하기 위해 .vs파일을 삭제하여 솔루션 파일을 실행할 경우 x64로 설정될 가능성이 있습니다.
	프로젝트를 반드시 x86으로 설정하여 주시기 바랍니다.

Vulkan API는 Window 개발 툴에 포함되어 있지 않습니다.

Vulkan API와 수학 라이브러리인 glm, 그리고 윈도우 제어 라이브러리인 glfw를 별도로 설치해야 실행됩니다.

Vulkan API는 LunaG Vulkan SDK 1.2.131.2를 설치하면 PATH가 생성됩니다.
해당 SDK를 설치하는 것으로 끝낼 수 있습니다.
(https://vulkan.lunarg.com/sdk/home#windows)

(주의)
	1.2.131.2를 설치하면 Third-Party인 std가 지정된 위치에 압축이 풀리므로 꼭 1.2.131.2를 사용하시거나
	별도로 stb 라이브러리 위치를 프로젝트에서 설정하여야 합니다.

glm은 다음 Git 주소에서 다운로드 하여야 합니다.
https://github.com/g-truc/glm/releases/tag/0.9.9.8

VkPractice에 포함된 EW_Engine_Vk는 glm 라이브러리를 다음과 같은 위치에서 링크합니다.
C:\glm

기본적으로 glfw를 사용하여 윈도우를 생성하고 있으므로 해당 라이브러리를 다음 주소에서 다운로드 합니다.
https://www.glfw.org/

해당 라이브러리를 C:\glfw-3.3.2.bin.WIN32 경로에 위치시키는 것으로 준비가 끝납니다.
