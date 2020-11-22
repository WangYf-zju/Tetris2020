#ifndef TETRISAI_H
#define TETRISAI_H

#define ROW 12
#define COL 10
#define DEFALUT_COLOR 8
typedef int BoardStateArray[ROW][COL];
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
    int* operator[](int i) { return bsa[i]; }
private:
    BoardStateArray bsa;
};

class TetrisAI
{
public:
    /* constructor and destructor */
    /* Construct a new tetris and indicate type.
       Function immediatelly calculate the supreme postion of tetris
       but the tetris will not be placed on board.
       Parameter type should be a number between 0 to 6. */
    TetrisAI(int type);
    // destructor
    ~TetrisAI();

    /* public static variables */
    // static tetris shape array pointer
    static const bool(*TetrisShape)[4][16];

private:
    /* private static variables */
    // static board pointer
    static BoardState * PBoard;

    /* private member variables */
    // tetris type
    int m_type;
    // tetris supreme rank placed on board
    double m_rank;
    // flag indicate if the tetris can be placed on board
    bool m_canPlace;
    // supreme position pointer
    TetrisPosition *m_supPos;
    // array to save connected grid region size
    ConnectRegion m_CRGridCount;
    // array to save number of x-grid region (x = 1, 2, 3, 4 and 5)
    int m_CRCount[5];

public:
    /* public methods */
    // Get supreme postion of tetirs.
    TetrisPosition* GetSupremePos();
    // Place the tetris to its supreme position which means the board will be modified.
    TetrisPosition* PlaceToSupremePos();
    // Get supreme rank of tetris.
    double GetSupremeRank();
    // Bind the board pointer to another board.
    void BindBoard(BoardState * pBoard);
    // Place the tetris to a specific position.
    void PlaceTetris(int x, int y, int r);
    // Place the tetris to a specific position.
    void PlaceTetris(TetrisPosition * pos);
    // Remove the tetris from a specific position on board.
    void RemoveTetris(int x, int y, int r);

    /* public static methods */
    // Get board pointer
    static BoardState * GetBoard();
    // Place a tetris to a specific position.
    static void PlaceTetris(int type, TetrisPosition * pos);
    // Get supreme rank of a specific tetris.
    static double GetSupremeRank(int type);
    // Reverse the state of a grid on board.
    static void ReverseBlock(int x, int y);
    // Set the state of a grid on board.
    static void SetBlock(int x, int y, int color);
    // Remove the block from a grid on board.
    static void RemoveBlock(int x, int y);
    // Clear the board (Remove all the blocks on board).
    static void ClearBoard();

    /* private methods */
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