#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include <iostream>
#include <algorithm>
#include <random>
//#include <cstdlib>
//#include <cstdlib>

class game15 : public olc::PixelGameEngine
{
public:
    explicit game15(std::string fileName);
    bool OnUserCreate() override;
    void PuzzleStep();
    bool OnUserUpdate(float fElapsedTime) override;
    int width() const
    {
        return pic.width;
    }
    int height() const
    {
        return pic.height;
    }
private:
    void DrawField()
    {
        for (int i = 0; i < _countof(table.table); ++i)
        {
            int x = (table[i] % 4) * width() / 4;
            int y = (table[i] / 4) * height() / 4;
            if (i == 15)
            {
                FillRect(x, y, width() / 4, height() / 4, olc::BLACK);
                continue;
            }
            DrawSprite(x, y, cells[i], 1);
            DrawRect(x + 1, y + 1, width() / 4 - 2, height() / 4 - 2, table[i] == i ? olc::GREEN : olc::RED);
        }
    }

    void Restart()
    {
        for (short i = 0; i < _countof(table.table); ++i)
        {
            table.table[i] = i;
            table.antitable[i] = i;
        }

        table.Shuffle();
    }

    bool Solved()
    {
        for (int i = 0; i < _countof(table.table); ++i)
        {
            if (i != table[i])
            {
                return false;
            }
        }
        return true;
    }

    struct Table
    {
        short table[16];
        short antitable[16];
        short operator [](int i)
        {
            return table[i];
        }
        void swap(int pos1, int pos2)
        {
            const int ind1 = antitable[pos1];
            const int ind2 = antitable[pos2];
            std::swap(table[ind1], table[ind2]);
            std::swap(antitable[pos1], antitable[pos2]);
        }

        void Shuffle()
        {
            std::shuffle(table, table + _countof(table), std::mt19937(std::random_device()()));

            for (int i = 0; i < _countof(table); ++i)
            {
                antitable[table[i]] = i;
            }

            int inverses = 0;
            for (int i = 0; i < _countof(table); ++i)
            {
                for (int j = i + 1; j < _countof(table); ++j)
                {
                    if (antitable[i] != 15 && antitable[j] != 15 && antitable[i] > antitable[j])
                    {
                        ++inverses;
                    }
                }
            }

            inverses += table[15] / 4 + 1;

            if (inverses % 2 == 1)
            {
                int i = 0;
                while (antitable[i] == 15 || antitable[i + 1] == 15)
                    ++i;
                swap(i, i + 1);
            }
        }
    };

    Table table;
    olc::Sprite pic;
    olc::Sprite * cells[16];
};

game15::game15(std::string fileName) : pic(fileName)
{
    Restart();

    pic.width -= pic.width % 4;
    pic.height -= pic.height % 4;

    int cell_w = pic.width / 4;
    int cell_h = pic.height / 4;

    for (short i = 0; i < _countof(cells); ++i)
    {
        cells[i] = new olc::Sprite(cell_w, cell_h);
        for (int k = 0; k < cell_h; ++k)
        {
            for (int l = 0; l < cell_w; ++l)
            {
                cells[i]->SetPixel(l, k, pic.GetPixel((i % 4) * cell_w + l, (i / 4) * cell_h + k));
            }
        }
    }

    Construct(pic.width, pic.height, 1, 1);
    Start();
}

bool game15::OnUserCreate()
{
    DrawField();
    return true;
}

void game15::PuzzleStep()
{
    if (GetMouse(0).bReleased)
    {
        int mx = GetMouseX() / (width() / 4);
        int my = GetMouseY() / (height() / 4);
        int mpos = my * 4 + mx;
        //std::cerr << mx << my << " " << mpos << " " << table.antitable[mpos] << std::endl;
        int ex = table[15] % 4;
        int ey = table[15] / 4;
        if (mx == ex && my == ey)
        {
            return;
        }
        if (mx == ex)
        {
            int sign = my > ey ? 1 : -1;
            for (int iy = ey + sign; iy != my + sign; iy += sign)
            {
                table.swap(iy * 4 + mx, table[15]);
            }
        }
        else if (my == table[15] / 4)
        {
            int sign = mx > ex ? 1 : -1;
            for (int ix = ex + sign; ix != mx + sign; ix += sign)
            {
                table.swap(my * 4 + ix, table[15]);
            }
        }
        DrawField();
    }

}

bool game15::OnUserUpdate(float fElapsedTime)
{
    PuzzleStep();
    if (Solved())
    {
        Restart();
        DrawField();
    }
    return true;
}


int main(int args, char ** argv)
{
    std::string filename;
    if(args == 2)
    {
        filename = argv[1];
    }
    else
    {
        filename = std::string("15_default.bmp");
    }

    game15 game(filename);
    return 0;
}
