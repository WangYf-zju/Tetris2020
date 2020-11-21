#include "TetrisAI.h"

// Global tetris shape array
const bool TetrisShapeArray[7][4][16] =
{
{{1,0,0,0,1,1,1},{0,1,1,0,0,1,0,0,0,1},{0,0,0,0,1,1,1,0,0,0,1},{0,1,0,0,0,1,0,0,1,1}},
{{0,0,1,0,1,1,1},{0,1,0,0,0,1,0,0,0,1,1},{0,0,0,0,1,1,1,0,1},{1,1,0,0,0,1,0,0,0,1}},
{{0,1,1,0,1,1},{1,0,0,0,1,1,0,0,0,1},{0,1,1,0,1,1},{1,0,0,0,1,1,0,0,0,1}},
{{1,1,0,0,0,1,1},{0,0,1,0,0,1,1,0,0,1},{1,1,0,0,0,1,1},{0,0,1,0,0,1,1,0,0,1}},
{{1,1,0,0,1,1},{1,1,0,0,1,1},{1,1,0,0,1,1},{1,1,0,0,1,1}},
{{0,0,0,0,1,1,1,1},{0,1,0,0,0,1,0,0,0,1,0,0,0,1},
    {0,0,0,0,1,1,1,1},{0,1,0,0,0,1,0,0,0,1,0,0,0,1}},
{{0,1,0,0,1,1,1},{0,1,0,0,0,1,1,0,0,1},{0,0,0,0,1,1,1,0,0,1},{0,1,0,0,1,1,0,0,0,1}}
};
// Global board state variable
BoardState board;
// Global tetris shape pointerd
const bool(*TetrisAI::TetrisShape)[4][16] = TetrisShapeArray;
BoardState* TetrisAI::PBoard = &board;

TetrisAI::TetrisAI(int type)
{
    if (type < 0 || type > 6)
    {
        m_type = 0;
        throw "TetrisAI: parameter type should be a number between 0 to 6";
    }
    else
    {
        m_type = type;
    }
    m_supPos = new TetrisPosition;
    m_supPos->x = 0;
    m_supPos->y = 0;
    m_supPos->r = 0;
    m_rank = -1e10;
    FindSupremePos();
}

TetrisAI::~TetrisAI()
{
    delete m_supPos;
}


TetrisPosition* TetrisAI::GetSupremePos()
{
    if (!m_canPlace)
        return nullptr;
    return m_supPos;
}

TetrisPosition* TetrisAI::PlaceToSupremePos()
{
    TetrisPosition * pos = GetSupremePos();
    if (nullptr == pos)
        return nullptr;
    PlaceTetris(m_supPos);
    return m_supPos;
}

double TetrisAI::GetSupremeRank()
{
    return m_rank;
}

bool TetrisAI::FindSupremePos()
{
    bool canPlace = false;
    double max = -1e10;
    for (int x = -1; x < COL; x++)
    {
        for (int y = -1; y < ROW; y++)
        {
            for (int r = 0; r < 4; r++)
            {
                if (CanPlaceTetris(x, y, r))
                {
                    canPlace = true;
                    double rank = Rank(x, y, r);
                    if (rank > max)
                    {
                        max = rank;
                        m_rank = rank;
                        m_supPos->x = x;
                        m_supPos->y = y;
                        m_supPos->r = r;
                    }
                }
            }
        }
    }
    m_canPlace = canPlace;
    return canPlace;
}

bool TetrisAI::CanPlaceTetris(int x, int y, int r)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (TetrisShape[m_type][r][j * 4 + i] && !CanPlaceBlock(x + i, y + j))
                return false;
        }
    }
    return true;
}


bool TetrisAI::CanPlaceBlock(int x, int y)
{
    if (x < 0 || x >= COL || y < 0 || y >= ROW || (*PBoard)[y][x] != 0)
        return false;
    return true;
}


void TetrisAI::BindBoard(BoardState * pBoard)
{
    PBoard = pBoard;
}


double TetrisAI::Rank(int x, int y, int r)
{
    int LH = CalcLandHeight(y, r);
    int oldER = CalcEroded();
    int oldHole = CalcHoles();
    int oldRT = CalcRowTrans();
    int oldCT = CalcColTrans();
    CalcCR();
    int oldCR1 = m_CRCount[0];
    int oldCR2 = m_CRCount[0];
    int oldCR3 = m_CRCount[0];
    PlaceTetris(x, y, r);
    int dER = CalcEroded() - oldER;
    int dHole = CalcHoles() - oldHole;
    int dRT = CalcRowTrans() - oldRT;
    int dCT = CalcColTrans() - oldCT;
    CalcCR();
    int dCR1 = m_CRCount[0] - oldCR1;
    int dCR2 = m_CRCount[1] - oldCR2;
    int dCR3 = m_CRCount[2] - oldCR3;
    RemoveTetris(x, y, r);
    double rank = -5 * LH + 10 * dER - 3 * dRT - 5 * dCT - 5 * dHole
        - 50 * dCR1 - 75 * dCR2 - 100 * dCR3;
    return rank;
}


int TetrisAI::CalcLandHeight(int y, int r)
{
    for (int i = 3; i >= 0; i--)
    {
        for (int j = 0; j < 4; j++)
        {
            if (TetrisShape[m_type][r][i * 4 + j])
            {
                return y + i;
            }
        }
    }
    return y;
}


int TetrisAI::CalcEroded()
{
    bool flag;
    int count = 0;
    for (int y = 0; y < ROW; y++)
    {
        flag = true;
        for (int x = 0; x < COL; x++)
        {
            if (!(*PBoard)[y][x])
                flag = false;
        }
        if (flag) count++;
    }
    return count;
}


int TetrisAI::CalcHoles()
{
    bool flag;
    int count = 0;
    for (int x = 0; x < ROW; x++)
    {
        flag = false;
        for (int y = COL - 1; y >= 0; y--)
        {
            if ((*PBoard)[y][x]) flag = true;
            if (flag && !(*PBoard)[y][x]) count++;
        }
    }
    return 0;
}


void TetrisAI::PlaceTetris(int x, int y, int r)
{
    int color = m_type + 2;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (TetrisShape[m_type][r][4 * j + i])
            {
                SetBlock(x + i, y + j, color);
            }
        }
    }
}

void TetrisAI::PlaceTetris(TetrisPosition * pos)
{
    PlaceTetris(pos->x, pos->y, pos->r);
}


void TetrisAI::RemoveTetris(int x, int y, int r)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (TetrisShape[m_type][r][4 * j + i])
            {
                RemoveBlock(x + i, y + j);
            }
        }
    }
}

BoardState * TetrisAI::GetBoard()
{
    return PBoard;
}

void TetrisAI::PlaceTetris(int type, TetrisPosition * pos)
{
    int color = type + 2;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (TetrisShape[type][pos->r][4 * j + i])
            {
                SetBlock(pos->x + i, pos->y + j, color);
            }
        }
    }
}

double TetrisAI::GetSupremeRank(int type)
{
    TetrisAI ta(type);
    return ta.GetSupremeRank();
}

void TetrisAI::ReverseBlock(int x, int y)
{
    (*PBoard)[y][x] = !(*PBoard)[y][x];
}

void TetrisAI::SetBlock(int x, int y, int color)
{
    (*PBoard)[y][x] = color;
}

void TetrisAI::RemoveBlock(int x, int y)
{
    (*PBoard)[y][x] = 0;
}

void TetrisAI::ClearBoard()
{
    for (int i = 0; i < COL; i++)
    {
        for (int j = 0; j < ROW; j++)
        {
            RemoveBlock(i, j);
        }
    }
}

int TetrisAI::CalcRowTrans()
{
    int count = 0;
    for (int y = 0; y < ROW; y++)
    {
        if (!(*PBoard)[0][y]) count++;
        for (int x = 1; x < COL; x++)
        {
            if ((*PBoard)[y][x] != (*PBoard)[y][x - 1])
                count++;
        }
        if (!(*PBoard)[y][COL - 1]) count++;
    }
    return count;
}


int TetrisAI::CalcColTrans()
{
    int count = 0;
    for (int x = 0; x < COL; x++)
    {
        if (!(*PBoard)[0][x]) count++;
        for (int y = 1; y < ROW; y++)
        {
            if ((*PBoard)[y][x] != (*PBoard)[y - 1][x])
                count++;
        }
        if (!(*PBoard)[ROW - 1][x]) count++;
    }
    return count;
}


void TetrisAI::CalcCR()
{
    // Clear array.
    for (int i = 0; i < 50; i++)
    {
        m_CRGridCount[i] = 0;
    }
    for (int i = 0; i < 5; i++)
    {
        m_CRCount[i] = 0;
    }
    // Use connected region algorithm to count x-gird region.
    int order = 0;
    int hOrder = 0;
    int boardFlag[ROW][COL] = { 0 };
    int oldCRStart[5] = { -1,-1,-1,-1,-1 };
    int oldCREnd[5] = { -1,-1,-1,-1,-1 };
    int CRStart[5] = { -1,-1,-1,-1,-1 };
    int CREnd[5] = { -1,-1,-1,-1,-1 };
    for (int y = 0; y < ROW; y++)
    {
        for (int x = 0; x < COL + 1; x++)
        {
            if (CanPlaceBlock(x, y) && !CanPlaceBlock(x - 1, y))
            {
                CRStart[hOrder] = x;
                CREnd[hOrder] = x;
            }
            else if (CanPlaceBlock(x, y) && CanPlaceBlock(x - 1, y))
            {
                CREnd[hOrder] = x;
            }
            else if (!CanPlaceBlock(x, y) && CanPlaceBlock(x - 1, y))
            {
                bool flag = false;
                int curOrder = 0;
                for (int i = 0; i < 5; i++)
                {
                    if ((CRStart[hOrder] >= oldCRStart[i] && CRStart[hOrder] <= oldCREnd[i])
                        || (CREnd[hOrder] >= oldCRStart[i] && CREnd[hOrder] <= oldCREnd[i])
                        || (oldCRStart[i] >= CRStart[hOrder] && oldCRStart[i] <= CREnd[hOrder])
                        || (oldCREnd[i] >= CRStart[hOrder] && oldCREnd[i] <= CREnd[hOrder]))
                    {
                        if (!flag)
                        {
                            curOrder = boardFlag[y - 1][oldCRStart[i]];
                            for (int j = CRStart[hOrder]; j <= CREnd[hOrder]; j++)
                            {
                                boardFlag[y][j] = curOrder;
                            }
                        }
                        else
                        {
                            int curModOrder = boardFlag[y - 1][oldCRStart[i]];
                            for (int yy = 0; yy <= y; yy++)
                            {
                                for (int xx = 0; xx < COL; xx++)
                                {
                                    if (boardFlag[yy][xx] == curModOrder)
                                        boardFlag[yy][xx] = curOrder;
                                }
                            }
                        }
                        flag = true;
                    }
                }
                if (!flag)
                {
                    order++;
                    for (int j = CRStart[hOrder]; j <= CREnd[hOrder]; j++)
                    {
                        boardFlag[y][j] = order;
                    }
                }
                hOrder++;
            }
        }
        for (int i = 0; i < 5; i++)
        {
            oldCRStart[i] = CRStart[i];
            oldCREnd[i] = CREnd[i];
            CRStart[i] = -1;
            CREnd[i] = -1;
        }
        hOrder = 0;
    }
    for (int y = 0; y < ROW; y++)
    {
        for (int x = 0; x < COL; x++)
        {
            if (boardFlag[y][x] > 0)
                m_CRGridCount[boardFlag[y][x] - 1]++;
        }
    }
    for (int i = 0; i < 50; i++)
    {
        switch (m_CRGridCount[i] % 4)
        {
        case 1:
            m_CRCount[0]++;
            break;
        case 2:
            m_CRCount[1]++;
            break;
        case 3:
            m_CRCount[2]++;
            break;
        default:
            break;
        }
    }
}
