#include "RaySim/RaySim.hpp"

class MySimulation : public RS::Canvas {
public:
    void Setup() override {
        SetSize(800, 450);
        SetTitle("RaySim - Basic Example");
        SetFrameRate(60);
    }

    void Draw() override {
        Background(RS::DARKGRAY);

        // Logic to draw on the screen
        DrawText("Congrats! You created your first window!", 190, 200, 20, RS::LIGHTGRAY);
    }
};

int main()
{
    MySimulation sim;
    sim.Run();

    return 0;
}
