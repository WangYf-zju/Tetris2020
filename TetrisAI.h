#ifndef TETRISAI_H
#define TETRISAI_H

#define ROW 12
#define COL 10
typedef bool BoardStateArray[ROW][COL];
typedef int ConnectRegion[50];

typedef struct {
    int x;
    int y;
    int r;
    int type;
}TetrisPosition;

class BoardState
{
public:
    bool * operator[](const int & i) { return bsa[i]; }
private:
    BoardStateArray bsa;
};

class TetrisAI
{
public:
    TetrisAI(int type);
    ~TetrisAI();
public:
    int m_type;
    double m_rank;
    bool m_canPlace;
    TetrisPosition *m_supPos;
    static const bool(*TetrisShape)[4][16];
    static BoardState * m_pBoard;
private:
    ConnectRegion m_CRGridCount;
    int m_CRCount[5];

public:
    TetrisPosition* GetSupremePos();
    TetrisPosition* PlaceToSupremePos();
    double GetSupremeRank();
    void BindBoard(BoardState * pBoard);
    void PlaceTetris(int x, int y, int r);
    void PlaceTetris(TetrisPosition * pos);
    void RemoveTetris(int x, int y, int r);
    static void PlaceTetris(int type, TetrisPosition * pos);
    static void ReverseBlock(int x, int y);
    static void SetBlock(int x, int y);
    static void RemoveBlock(int x, int y);
    static void ClearBoard();
private:
    bool FindSupremePos();
    bool CanPlaceTetris(int x, int y, int r);
    bool CanPlaceBlock(int x, int y);
    double Rank(int x, int y, int r);
    int CalcLandHeight(int y, int r);
    int CalcEroded();
    int CalcHoles();
    int CalcRowTrans();
    int CalcColTrans();
    void CalcCR();
};

#endif // !TETRISAI_H