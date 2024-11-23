#include <string>
#include <raylib.h>
#include <utility>
#include <vector>

class Box {
    public:
    Rectangle rect;
    Color color;
    std::string name;

    Box(Rectangle rect, Color color, std::string name)
        : rect(rect), color(color), name(std::move(name)) {}

    void display() const {
        DrawRectangleRec(rect, color);
        DrawText(name.c_str(), (int)(rect.x + rect.width - MeasureText(name.c_str(), 10) - 12),
            (int)(rect.y + rect.height - 20), 10, WHITE);
    }
};


int main() {
    // Initialize window
    const int screenWidth = 800;
    const int screenHeight = 800;
    InitWindow(screenWidth, screenHeight, "Drag and Drop Example");

    // Selection zone and canvas dimensions
    Rectangle selectionZone = { 50, 50, 700, 500 };
    Rectangle canvas = { 50, 600, 700, 150 };
    std::vector<Box> boxes;


    // Create boxes
    std::vector<std::string> boxNames = {
        "First item", "Second item", "Third item"
    };
    
    const std::size_t num_boxes = boxNames.size();

    int selectedBox = -1;  // Pointer to the currently dragged box
    Box dragBox({0, 0, 100, 100}, RED, "");  // Pointer to the currently dragged box
    Vector2 mousePoint = { 0.0f, 0.0f };


    Rectangle colorsRecs[num_boxes];     // Rectangles array
    std::vector colorState(num_boxes, 0);

    // Fills colorsRecs data (for every rectangle)
    for (int i = 0; i < num_boxes; i++)
    {
        colorsRecs[i].x = 70 + 120.0f*i;
        colorsRecs[i].y = 80.0f;
        colorsRecs[i].width = 100.0f;
        colorsRecs[i].height = 100.0f;
    }

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // Get mouse position
        mousePoint = GetMousePosition();

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            selectedBox = -1;

            if (CheckCollisionPointRec(mousePoint, canvas)) {
                if (boxes.empty()) {
                    boxes.push_back(Box({canvas.x + 20, canvas.y + 20, 100, 100}, RED, dragBox.name));
                }
                else {
                    Rectangle prevRect = boxes[boxes.size() - 1].rect;
                    boxes.push_back(Box({prevRect.x + prevRect.width + 20, prevRect.y, prevRect.width, prevRect.height} , RED, dragBox.name));
                }
            }
        }

        for (int i = 0; i < num_boxes; i++)
        {
            if (CheckCollisionPointRec(mousePoint, colorsRecs[i])) {
                colorState[i] = 1;
                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                    selectedBox = i;
                }
            }
            else colorState[i] = 0;
        }

        // Start drawing
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw selection zone
        DrawRectangleRec(selectionZone, LIGHTGRAY);
        DrawText("Selection Zone", selectionZone.x + 10, selectionZone.y + 10, 10, DARKGRAY);

        // Draw canvas
        DrawRectangleRec(canvas, BEIGE);
        DrawText("Canvas", canvas.x + 10, canvas.y + 10, 10, DARKGRAY);


        for (int i = 0; i < num_boxes; i++) {    // Draw all rectangles
                DrawRectangleRec(colorsRecs[i], RED);

                if (IsKeyDown(KEY_SPACE) || colorState[i])
                {
                    DrawRectangle((int)colorsRecs[i].x, (int)(colorsRecs[i].y + colorsRecs[i].height - 26), (int)colorsRecs[i].width, 20, BLACK);
                    DrawRectangleLinesEx(colorsRecs[i], 6, Fade(BLACK, 0.3f));
                    DrawText(boxNames[i].c_str(), (int)(colorsRecs[i].x + colorsRecs[i].width - MeasureText(boxNames[i].c_str(), 10) - 12),
                        (int)(colorsRecs[i].y + colorsRecs[i].height - 20), 10, WHITE);
                }
        }

        for (auto& box : boxes) {
            box.display();
        }

        if (selectedBox != -1) {
            dragBox.rect.x = mousePoint.x - dragBox.rect.width / 2;
            dragBox.rect.y = mousePoint.y - dragBox.rect.height / 2;
            dragBox.name = boxNames[selectedBox];
            dragBox.display();
        }


        EndDrawing();
    }

    CloseWindow();
    return 0;
}
