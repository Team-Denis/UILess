#include <colors.h>
#include <raylib.h>

const int padding = 20;

// TODO: Put in cpp/h files
// TODO: Rely less on hardcoded button sizes or at least make it a constant
namespace ImGui {

void draw_button(Texture emoji, Vector2 pos) {
  DrawRectangleRounded(Rectangle{pos.x, pos.y, 90, 70}, 0.1f, 20, Colors::BLUE2);
  DrawTexturePro(emoji,
                 Rectangle{0, 0, static_cast<float>(emoji.width),
                           static_cast<float>(emoji.height)},
                 Rectangle{20 + pos.x, 10 + pos.y, 50, 50}, Vector2{0, 0}, 0,
                 WHITE);
}

bool draw_run_button(Vector2 center, float radius) {
  auto mouse = GetMousePosition();
  bool does_collide = CheckCollisionPointCircle(mouse, center, radius);

  DrawCircleV(center, radius, Colors::GREEN1);

  return IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && does_collide;
}

class SidePanel {
public:
  explicit SidePanel(Rectangle frame) {
    m_frame = frame;
    m_at = Vector2{m_frame.x + padding, m_frame.y + padding};

    DrawRectangleRounded(frame, 0.04f, 20, Colors::BG2);
  }

  void push_button(Texture emoji) {
    if (m_at.x + padding + 90 > m_frame.width + m_frame.x) {
      m_at.x = m_frame.x + padding;
      m_at.y += 70 + padding;
    }

    draw_button(emoji, m_at);

    m_at.x += 90 + padding;
  }

private:
  Rectangle m_frame{};
  Vector2 m_at{};
};

};

Texture load_texture(char const *fileName) {
  Texture tex = LoadTexture(fileName);
  GenTextureMipmaps(&tex);
  SetTextureFilter(tex, TEXTURE_FILTER_BILINEAR);

  return tex;
}

class CommandLine {
public:
  explicit CommandLine(Rectangle frame) {
    DrawRectangleRounded(frame, 0.04f, 20, Colors::BG3);
  }
};

int main(int argc, char **argv) {
  SetConfigFlags(FLAG_MSAA_4X_HINT);

  InitWindow(1280, 800, "UILess");

  SetWindowState(FLAG_VSYNC_HINT);

  //  RenderTexture blur_texture = LoadRenderTexture(100, 100);
  //  Shader shader = LoadShader(nullptr, TextFormat("shaders/blur.glsl", 330));

  auto file = load_texture("assets/file.png");

  //  int resolutionLoc = GetShaderLocation(shader, "resolution");

  while (!WindowShouldClose()) {
    BeginDrawing();

    ClearBackground(Colors::BG1);

    // Side panel

    Rectangle side_panel_frame{padding, padding, (padding + 3 * (90 + padding)),
                               static_cast<float>(GetRenderHeight()) - 2 * padding};

    auto side_panel = ImGui::SidePanel(side_panel_frame);

    side_panel.push_button(file);
    side_panel.push_button(file);
    side_panel.push_button(file);
    side_panel.push_button(file);
    side_panel.push_button(file);
    side_panel.push_button(file);
    side_panel.push_button(file);

    {
      Rectangle panel{side_panel_frame.width + side_panel_frame.x + padding, padding,
                      static_cast<float>(GetRenderWidth()) -
                          side_panel_frame.width - side_panel_frame.x - 2 * padding,
                      static_cast<float>(GetRenderHeight() - 2 * padding)};
      DrawRectangleRounded(panel, 0.02f, 20, Colors::BG2);

      const int run_button_radius = 40;

      Rectangle cmd_panel_frame{ panel.x + padding, panel.y + panel.height - 100 - padding, panel.width - 2 * padding - padding - 2 * run_button_radius,  100 };

      DrawRectangleRounded(cmd_panel_frame, 0.15f, 20, Colors::BG3);

      Vector2 run_button_center{panel.x + panel.width - padding - run_button_radius, cmd_panel_frame.y + run_button_radius + 10};

      if (ImGui::draw_run_button(run_button_center, run_button_radius)) {
        TraceLog(TraceLogLevel::LOG_INFO, "Run button clicked");
      }
    }

    EndDrawing();
  }
}