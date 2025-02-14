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
        bool isDragging = false;               // 判断是否在拖拽
        int lastMouseX = 0, lastMouseY = 0;    // 记录上次鼠标位置
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
                    if (!isDragging) {  // 如果还没有拖拽，记录初始点
                        isDragging = true;
                        lastMouseX = args.position.X;
                        lastMouseY = args.position.Y;
                    }
                    else {  // 已经在拖拽时，计算鼠标的移动距离并更新位置
                        int deltaX = args.position.X - lastMouseX;
                        int deltaY = args.position.Y - lastMouseY;

                        X += deltaX;  // 更新图像位置
                        Y += deltaY;

                        lastMouseX = args.position.X;  // 更新上次鼠标位置
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
            // 立方体的顶点（未旋转，单位立方体）
            std::vector<std::vector<float>> vertices = {
                {-1.0f, -1.0f, -1.0f}, { 1.0f, -1.0f, -1.0f}, { 1.0f,  1.0f, -1.0f}, {-1.0f,  1.0f, -1.0f},
                {-1.0f, -1.0f,  1.0f}, { 1.0f, -1.0f,  1.0f}, { 1.0f,  1.0f,  1.0f}, {-1.0f,  1.0f,  1.0f}
            };

            // 旋转矩阵：绕X轴
            auto rotateX = [&](float& x, float& y, float& z) {
                float cosA = cos(angleX);
                float sinA = sin(angleX);
                float newY = y * cosA - z * sinA;
                float newZ = y * sinA + z * cosA;
                y = newY;
                z = newZ;
            };

            // 旋转矩阵：绕Y轴
            auto rotateY = [&](float& x, float& y, float& z) {
                float cosA = cos(angleY);
                float sinA = sin(angleY);
                float newX = x * cosA + z * sinA;
                float newZ = -x * sinA + z * cosA;
                x = newX;
                z = newZ;
            };

            // 旋转矩阵：绕Z轴
            auto rotateZ = [&](float& x, float& y, float& z) {
                float cosA = cos(angleZ);
                float sinA = sin(angleZ);
                float newX = x * cosA - y * sinA;
                float newY = x * sinA + y * cosA;
                x = newX;
                y = newY;
            };

            // 旋转立方体的顶点
            for (auto& vertex : vertices) {
                rotateX(vertex[0], vertex[1], vertex[2]);
                rotateY(vertex[0], vertex[1], vertex[2]);
                rotateZ(vertex[0], vertex[1], vertex[2]);
            }

            // 投影到2D平面，显示到屏幕上
            std::vector<std::pair<int, int>> screenVertices;
            for (const auto& vertex : vertices) {
                // 使用 width 和 height 控制比例，不再使用 size
                int x = static_cast<int>((vertex[0] + 1.0f) * (width * 0.5f)) + centerX;  // 缩放到屏幕宽度
                int y = static_cast<int>((vertex[1] + 1.0f) * (height * 0.5f)) + centerY; // 缩放到屏幕高度

                screenVertices.push_back({ x, y });
            }

            // 定义立方体的边（顶点连接）
            std::vector<std::pair<int, int>> edges = {
                {0, 1}, {1, 2}, {2, 3}, {3, 0}, // 底面
                {4, 5}, {5, 6}, {6, 7}, {7, 4}, // 顶面
                {0, 4}, {1, 5}, {2, 6}, {3, 7}  // 侧面
            };

            Pixel pixel = { 200, 200, 200, 255 }; // 白色

            // 绘制边
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