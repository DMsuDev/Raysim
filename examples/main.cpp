#include "RaySim/RaySim.hpp"

class MySimulation : public RS::Canvas {
public:
    void Setup() override {
        SetSize(800, 450);
        SetTitle("RaySim - Basic Example");
        SetDefaultFont("resources/fonts/opensans/OpenSans-Regular.ttf", 64);
        SetFrameRate(60);
    }

    void Draw() override {
        Background(RS::DARKGRAY);

        DrawText("Congrats! You created your first window!", 190, 200, 30, RS::LIGHTGRAY);
    }
};

int main()
{
    MySimulation sim;
    sim.Run();

    return 0;
}
