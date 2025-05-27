#include "move_validator.cpp" 

using namespace MoveValidator;

bool isvalidNotation(const std::string& move){
    static const std::regex sanPattern(
        R"(^(
            O-O(-O)?                             |  # Castling
            ([KQRNB])?                           # Optional piece letter
            ([a-h]|[1-8]|[a-h][1-8])?            # Optional disambiguation
            x?                                   # Optional capture
            [a-h][1-8]                           # Destination square
            (=)[QRNB]?                           # Optional promotion
            [+#]?                                # Optional check or mate
        )$)",
        std::regex::extended | std::regex::icase
    );
    return std::regex_match(move, sanPattern);
}

ParsedSAN parseSAN(const std::string& move){

}

bool pieceCanReach(const Piece& piece, int depRow, int depCol, int destRow, int destCol, const Board& board){
    if piece.name == "pawn"{
        if (!piece.isPromoted){
            if (piece.color == "black"){
                //black pawns can only increase in row
            } else{
                //white pawns can only decrease in row 
            }
            //Col must be +/- 1 of the depCol 
        } //else ... moves like Queen  
    } else if piece.name == "rook"{
        
    } else if piece.name == "knight"{

    } else if piece.name == "bishop"{

    } else if piece.name == "queen"{

    } else if piece.name == "king"{

    }
}

bool isvalidPattern(const Board&board, const std::string&move, const std::string&color){
    ParsedSAN parsed_move = parseSAN(move); 
    int target_col = parsed_move.destFile - 'a';
    int target_row = 8 - parsed_move.destRank;

    //want to iterate over the board to determine where the piece is coming from
    std::vector<std::shared_ptr<Piece>> candidate_pieces;
    for (int row=0; row<8; ++row){
        for (int col=0; col<8; ++col){
            auto piece = board.grid[row][col]; //remember this is a shared pointer
            if (!piece || piece->color != color) continue;
            if (piece->name != parsed_move.pieceType) continue;
            if (parsed.disambiguationFile && col != (parsed.disambiguationFile - 'a')) continue;
            if (parsed.disambiguationRank && row != (8 - (parsed.disambiguationRank - '0'))) continue;
            //we have found the piece that this move is referring to 
            if pieceCanReach(*piece,row,col,target_row,target_col,board){
                candidate_pieces.push_back(piece);
            }
        }
    }


}

MoveResult isvalidMove(const Board&board, const std::string&move, const std::string&color){
    //Step 1: Notation check 
    if (!isvalidNotation(move)){
        return MoveResult::error("ERROR: This is not a valid SAN move.");
    }
    //Step 2: Check validity of pattern
    if (!isvalidPattern(board,move,color)){
        return MoveResult::error("ERROR: That piece cannot move that way.");
    }
    //Step 3: Check legality of the move 
    if (!islegalMove(board,move,color)){
        return MoveResult::error("ERROR: This move is illegal.");
    }
    return MoveResult::success
}