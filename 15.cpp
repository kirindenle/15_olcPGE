#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include <iostream>
#include <algorithm>
#include <random>

const int FIELD_HEIGHT = 4;
const int TABLE_SIZE = FIELD_HEIGHT * FIELD_HEIGHT;
const int EMPTY_INDEX = TABLE_SIZE - 1;

class game15 : public olc::PixelGameEngine
{
public:
    explicit game15(std::string fileName);
    bool OnUserCreate() override;
    void PuzzleStep();
    bool OnUserUpdate(float fElapsedTime) override;
    int width() const
    {
        return pic_width;
    }
    int height() const
    {
        return pic_height;
    }
private:
    void DrawField()
    {
        //Clear(olc::BLACK);
        for (int i = 0; i < TABLE_SIZE; ++i)
        {
            int x = (table[i] % FIELD_HEIGHT) * width() / FIELD_HEIGHT;
            int y = (table[i] / FIELD_HEIGHT) * height() / FIELD_HEIGHT;
            if (i == EMPTY_INDEX)
            {
                FillRect(x, y, width() / FIELD_HEIGHT, height() / FIELD_HEIGHT, olc::BLACK);
                continue;
            }
            DrawSprite(x, y, cells[i], 1);
            DrawRect(x + 1, y + 1,
                cells[i]->width - 2, cells[i]->height - 2,
                table[i] == i ? olc::GREEN : olc::RED);
            DrawRect(x + 2, y + 2,
                cells[i]->width - 4, cells[i]->height - 4,
                table[i] == i ? olc::GREEN : olc::RED);
            
        }
    }

    void Restart()
    {
        for (short i = 0; i < TABLE_SIZE; ++i)
        {
            table.table[i] = i;
            table.antitable[i] = i;
        }

        table.Shuffle();
    }

    bool Solved()
    {
        for (int i = 0; i < TABLE_SIZE; ++i)
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
        short table[TABLE_SIZE];
        short antitable[TABLE_SIZE];
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
            std::shuffle(table, table + TABLE_SIZE, std::mt19937(std::random_device()()));

            for (int i = 0; i < TABLE_SIZE; ++i)
            {
                antitable[table[i]] = i;
            }

            int inverses = 0;
            for (int i = 0; i < TABLE_SIZE; ++i)
            {
                for (int j = i + 1; j < TABLE_SIZE; ++j)
                {
                    if (antitable[i] != EMPTY_INDEX 
                        && antitable[j] != EMPTY_INDEX 
                        && antitable[i] > antitable[j])
                    {
                        ++inverses;
                    }
                }
            }

            inverses += table[EMPTY_INDEX] / FIELD_HEIGHT + 1;

            if (inverses % 2 == 1)
            {
                int i = 0;
                while (antitable[i] == EMPTY_INDEX || antitable[i + 1] == EMPTY_INDEX)
                    ++i;
                swap(i, i + 1);
            }
        }
    };

    Table table;
    olc::Sprite pic;
    int pic_width;
    int pic_height;
    olc::Sprite * cells[TABLE_SIZE];
};

game15::game15(std::string fileName) : pic(fileName)
{
    Restart();

    pic_width = pic.width - pic.width % FIELD_HEIGHT;
    pic_height = pic.height - pic.height % FIELD_HEIGHT;

    int cell_w = pic_width / FIELD_HEIGHT;
    int cell_h = pic_height / FIELD_HEIGHT;

    for (short i = 0; i < TABLE_SIZE; ++i)
    {
        cells[i] = new olc::Sprite(cell_w, cell_h);
        for (int k = 0; k < cell_h; ++k)
        {
            for (int l = 0; l < cell_w; ++l)
            {
                cells[i]->SetPixel(l, k, 
                    pic.GetPixel((i % FIELD_HEIGHT) * cell_w + l, (i / FIELD_HEIGHT) * cell_h + k));
            }
        }
    }

    Construct(pic_width, pic_height, 1, 1);
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
        int mx = GetMouseX() / (width() / FIELD_HEIGHT);
        int my = GetMouseY() / (height() / FIELD_HEIGHT);
        int mpos = my * FIELD_HEIGHT + mx;
        //std::cerr << mx << my << " " << mpos << " " << table.antitable[mpos] << std::endl;
        int ex = table[EMPTY_INDEX] % FIELD_HEIGHT;
        int ey = table[EMPTY_INDEX] / FIELD_HEIGHT;
        if (mx == ex && my == ey)
        {
            return;
        }
        if (mx == ex)
        {
            int sign = my > ey ? 1 : -1;
            for (int iy = ey + sign; iy != my + sign; iy += sign)
            {
                table.swap(iy * FIELD_HEIGHT + mx, table[EMPTY_INDEX]);
            }
        }
        else if (my == table[EMPTY_INDEX] / FIELD_HEIGHT)
        {
            int sign = mx > ex ? 1 : -1;
            for (int ix = ex + sign; ix != mx + sign; ix += sign)
            {
                table.swap(my * FIELD_HEIGHT + ix, table[EMPTY_INDEX]);
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
