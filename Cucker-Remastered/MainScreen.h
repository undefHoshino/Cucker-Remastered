#pragma once
#include "Interface.h"
#include "FpsOverlay.h"
#include "version.h"
#include "Animator.h"
#include "EasingFunction.h"
#include "Widget.h"
class MainScreen : public Interface {
public:
    class Cube {
    public:
        Animator animator;
        int X = 30;
        int Y = 15;
        double Width = 10;
        double Height = 5;
        bool isDragging = false;               // �ж��Ƿ�����ק
        int lastMouseX = 0, lastMouseY = 0;    // ��¼�ϴ����λ��
    public:
        void Creation() {
            auto& timeline = animator.ConfigureTimeline("0");
            timeline.addSegment("angleX").push(150000, 0, GetEasingFunction<EasingFunction::Sine>(1)).push(0, 360);
            timeline.addSegment("angleY").push(150000, 0, GetEasingFunction<EasingFunction::Sine>(1)).push(0, 360);
            timeline.addSegment("angleZ").push(150000, 0, GetEasingFunction<EasingFunction::Sine>(1)).push(0, 360);
            animator.SetDrawer("0", nullptr, [this](ScreenA* screen, CanvasA* canvas, KeyFrameLoader loader, void* args) {
                double angleX = loader["angleX"];
                double angleY = loader["angleY"];
                double angleZ = loader["angleZ"];
                drawCube(canvas, angleX, angleY, angleZ, X, Y, Width, Height);
            });
            animator.ManageTriggers({}, { {"0",Animator::PlaybackActionPreset::LoopingPlayback(animator.getTimeAxis(), timeline, "0")} });
            animator.Initialize();
        }
        void MouseEvent(MouseEventArgs args) {
            if (InputCondition::Mouse::InArea(X, Y, Width, Height, args) || isDragging) {
                if (args.buttonState[0]) {
                    if (!isDragging) {  // �����û����ק����¼��ʼ��
                        isDragging = true;
                        lastMouseX = args.position.X;
                        lastMouseY = args.position.Y;
                    }
                    else {  // �Ѿ�����קʱ�����������ƶ����벢����λ��
                        int deltaX = args.position.X - lastMouseX;
                        int deltaY = args.position.Y - lastMouseY;

                        X += deltaX;  // ����ͼ��λ��
                        Y += deltaY;

                        lastMouseX = args.position.X;  // �����ϴ����λ��
                        lastMouseY = args.position.Y;
                    }
                }
                else {
                    isDragging = false;
                }
            }
        }
        void Render(ScreenA* screen, CanvasA* canvas) {
            Width = 0.125 * screen->getWidth();
            Height = 0.25 * screen->getHeight();
            animator.ManageTriggers({ {"0", true} });
            animator.DrawEach(screen, canvas, 0);
        }
        void Update() {
            animator.getTrigger().forEach();
        }
        void drawCube(CanvasA* canvas, float angleX, float angleY, float angleZ, int centerX, int centerY, int width, int height) {
            // ������Ķ��㣨δ��ת����λ�����壩
            std::vector<std::vector<float>> vertices = {
                {-1.0f, -1.0f, -1.0f}, { 1.0f, -1.0f, -1.0f}, { 1.0f,  1.0f, -1.0f}, {-1.0f,  1.0f, -1.0f},
                {-1.0f, -1.0f,  1.0f}, { 1.0f, -1.0f,  1.0f}, { 1.0f,  1.0f,  1.0f}, {-1.0f,  1.0f,  1.0f}
            };

            // ��ת������X��
            auto rotateX = [&](float& x, float& y, float& z) {
                float cosA = cos(angleX);
                float sinA = sin(angleX);
                float newY = y * cosA - z * sinA;
                float newZ = y * sinA + z * cosA;
                y = newY;
                z = newZ;
            };

            // ��ת������Y��
            auto rotateY = [&](float& x, float& y, float& z) {
                float cosA = cos(angleY);
                float sinA = sin(angleY);
                float newX = x * cosA + z * sinA;
                float newZ = -x * sinA + z * cosA;
                x = newX;
                z = newZ;
            };

            // ��ת������Z��
            auto rotateZ = [&](float& x, float& y, float& z) {
                float cosA = cos(angleZ);
                float sinA = sin(angleZ);
                float newX = x * cosA - y * sinA;
                float newY = x * sinA + y * cosA;
                x = newX;
                y = newY;
            };

            // ��ת������Ķ���
            for (auto& vertex : vertices) {
                rotateX(vertex[0], vertex[1], vertex[2]);
                rotateY(vertex[0], vertex[1], vertex[2]);
                rotateZ(vertex[0], vertex[1], vertex[2]);
            }

            // ͶӰ��2Dƽ�棬��ʾ����Ļ��
            std::vector<std::pair<int, int>> screenVertices;
            for (const auto& vertex : vertices) {
                // ʹ�� width �� height ���Ʊ���������ʹ�� size
                int x = static_cast<int>((vertex[0] + 1.0f) * (width * 0.5f)) + centerX;  // ���ŵ���Ļ���
                int y = static_cast<int>((vertex[1] + 1.0f) * (height * 0.5f)) + centerY; // ���ŵ���Ļ�߶�

                screenVertices.push_back({ x, y });
            }

            // ����������ıߣ��������ӣ�
            std::vector<std::pair<int, int>> edges = {
                {0, 1}, {1, 2}, {2, 3}, {3, 0}, // ����
                {4, 5}, {5, 6}, {6, 7}, {7, 4}, // ����
                {0, 4}, {1, 5}, {2, 6}, {3, 7}  // ����
            };

            Pixel pixel = { 200, 200, 200, 255 }; // ��ɫ

            // ���Ʊ�
            for (const auto& edge : edges) {
                if (edge.first < screenVertices.size() && edge.second < screenVertices.size()) {
                    int x1 = screenVertices[edge.first].first;
                    int y1 = screenVertices[edge.first].second;
                    int x2 = screenVertices[edge.second].first;
                    int y2 = screenVertices[edge.second].second;
                    canvas->DrawLine(x1, y1, x2, y2, pixel);
                }
            }
        }
    };
private:
	FpsOverlay fpsOverlay;
	Pixel baseColor = { {30,30,30,255},{255,255,255,255} };
	Pixel textColor = { {},{200,200,200,255} };
	int RectColumnX = 10;
    Cube cube;
public:
	void Creation(ScreenA* screen, CanvasA* canvas, LogOverlay* logger, Displayer* display) override {
		Interface::Creation(screen, canvas, logger, display);
        cube.Creation();
        screen->sendSignal(ScreenA::Signal::SetBasePixel, &baseColor);
	}
	void Render() override {
        cube.Render(screen, canvas);
		canvas->FillRect(0, 0, 10, screen->getWidth(), { {255,255,255,25},{} });
		canvas->DrawString(RectColumnX + 2, 1, GetTimeFormatted(), textColor);
		canvas->DrawString(RectColumnX + 2, 3, "Cucker-Remastered [" + VERSION + "]", textColor);
		canvas->DrawString(RectColumnX + 2, 4, "Copyright 2022-2025. (Private Unreleased)", textColor);
		canvas->DrawString(RectColumnX + 2, 6, "", textColor);

		fpsOverlay.Render(*screen, *canvas);
	}

	void Mouse(MouseEventArgs args) override {
        cube.MouseEvent(args);
	}
	void Key(KeyEventArgs args) override {

	}
	void BackgroundLogic() override {
        cube.Update();
	}
	std::string GetTimeFormatted();
    
};

MainScreen* MakeMainScreen();