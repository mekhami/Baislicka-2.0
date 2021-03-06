#include "defs.h"

int find_moves_pawn_ep(s_board *board, s_move *move_list)
{
  assert(board != NULL);
  assert(move_list != NULL);
  
  int num_moves = 0;
  uint64_t moves;
  int from;
  
  // ep
  if(board->ep)
  {
    moves = magic_moves_pawns(!board->turn, board->ep) & board->pieces[PAWNS] & board->colour[board->turn];
    
    while(moves)
    {
      from = __builtin_ctzll(moves);
      move_list[num_moves] = move_add(board, from, board->ep, EP, PAWNS);
      num_moves++;
      moves &= moves-1;
    }
  }
  
  return num_moves;
}

int find_moves_pawn_captures(s_board *board, s_move *move_list, uint64_t allowed)
{
  assert(board != NULL);
  assert(move_list != NULL);
  
  int num_moves = 0;
  uint64_t moves;
  int from;
  int to;
  uint64_t copy;
  
  copy = board->pieces[PAWNS] & board->colour[board->turn];
  while(copy)
  {
    from = __builtin_ctzll(copy);
    moves = magic_moves_pawns(board->turn, from) & allowed;
    
    while(moves)
    {
      to = __builtin_ctzll(moves);
      num_moves += move_add_pawn(board, &move_list[num_moves], from, to);
      moves &= moves-1;
    }
    copy &= copy-1;
  }
  
  return num_moves;
}

int find_moves_wP_quiet(s_board *board, s_move *move_list)
{
  assert(board != NULL);
  assert(move_list != NULL);
  
  int num_moves = 0;
  uint64_t moves;
  int to;
  
  // Up 1
  moves = ((board->pieces[PAWNS] & board->colour[WHITE])<<8) & (~(board->colour[WHITE]|board->colour[BLACK]));
  while(moves)
  {
    to = __builtin_ctzll(moves);
    num_moves += move_add_pawn(board, &move_list[num_moves], to-8, to);
    
    // Up 2
    if(to <= 23 && 16 <= to && (((uint64_t)1<<(to+8))&(~(board->colour[WHITE]|board->colour[BLACK]))))
    {
      move_list[num_moves] = move_add(board, to-8, to+8, DOUBLE_PAWN, PAWNS);
      num_moves++;
    }
    
    moves &= moves-1;
  }
  
  return num_moves;
}

int find_moves_bP_quiet(s_board *board, s_move *move_list)
{
  assert(board != NULL);
  assert(move_list != NULL);
  
  int num_moves = 0;
  uint64_t moves;
  int to;
  
  // Down 1
  moves = ((board->pieces[PAWNS] & board->colour[BLACK])>>8) & (~(board->colour[WHITE]|board->colour[BLACK]));
  while(moves)
  {
    to = __builtin_ctzll(moves);
    num_moves += move_add_pawn(board, &move_list[num_moves], to+8, to);
    
    // Down 2
    if(40 <= to && to <= 47 && (((uint64_t)1<<(to-8))&(~(board->colour[WHITE]|board->colour[BLACK]))))
    {
      move_list[num_moves] = move_add(board, to+8, to-8, DOUBLE_PAWN, PAWNS);
      num_moves++;
    }
    
    moves &= moves-1;
  }
  
  return num_moves;
}

int find_moves_kings_castles(s_board *board, s_move *move_list)
{
  assert(board != NULL);
  assert(move_list != NULL);
  
  int num_moves = 0;
  
  // castling
  if(board->turn == WHITE)
  {
    if((board->castling & wKSC) &&
       !((board->colour[WHITE]|board->colour[BLACK])&U64_F1) &&
       !((board->colour[WHITE]|board->colour[BLACK])&U64_G1) &&
       square_attacked(board, U64_E1, BLACK) == 0 &&
       square_attacked(board, U64_F1, BLACK) == 0 &&
       square_attacked(board, U64_G1, BLACK) == 0)
    {
      move_list[num_moves] = move_add(board, E1, G1, KSC, KINGS);
      num_moves++;
    }
    if((board->castling & wQSC) &&
       !((board->colour[WHITE]|board->colour[BLACK])&U64_B1) &&
       !((board->colour[WHITE]|board->colour[BLACK])&U64_C1) &&
       !((board->colour[WHITE]|board->colour[BLACK])&U64_D1) &&
       square_attacked(board, U64_E1, BLACK) == 0 &&
       square_attacked(board, U64_D1, BLACK) == 0 &&
       square_attacked(board, U64_C1, BLACK) == 0)
    {
      move_list[num_moves] = move_add(board, E1, C1, QSC, KINGS);
      num_moves++;
    }
  }
  else
  {
    if((board->castling & bKSC) &&
       !((board->colour[WHITE]|board->colour[BLACK])&U64_F8) &&
       !((board->colour[WHITE]|board->colour[BLACK])&U64_G8) &&
       square_attacked(board, U64_E8, WHITE) == 0 &&
       square_attacked(board, U64_F8, WHITE) == 0 &&
       square_attacked(board, U64_G8, WHITE) == 0)
    {
      move_list[num_moves] = move_add(board, E8, G8, KSC, KINGS);
      num_moves++;
    }
    if((board->castling & bQSC) &&
       !((board->colour[WHITE]|board->colour[BLACK])&U64_B8) &&
       !((board->colour[WHITE]|board->colour[BLACK])&U64_C8) &&
       !((board->colour[WHITE]|board->colour[BLACK])&U64_D8) &&
       square_attacked(board, U64_E8, WHITE) == 0 &&
       square_attacked(board, U64_D8, WHITE) == 0 &&
       square_attacked(board, U64_C8, WHITE) == 0)
    {
      move_list[num_moves] = move_add(board, E8, C8, QSC, KINGS);
      num_moves++;
    }
  }
  
  return num_moves;
}

int find_moves_kings(s_board *board, s_move *move_list, uint64_t allowed)
{
  assert(board != NULL);
  assert(move_list != NULL);
  
  int num_moves = 0;
  uint64_t moves;
  int to;
  
  uint64_t from = __builtin_ctzll(board->colour[board->turn] & board->pieces[KINGS]);
  moves = magic_moves_king(from) & allowed;
  while(moves)
  {
    to = __builtin_ctzll(moves);
    move_list[num_moves] = add_movecapture(board, from, to, KINGS);
    num_moves++;
    moves &= moves-1;
  }
  
  return num_moves;
}

int find_moves_knights(s_board *board, s_move *move_list, uint64_t allowed)
{
  assert(board != NULL);
  assert(move_list != NULL);
  
  int num_moves = 0;
  uint64_t moves;
  int from;
  int to;
  uint64_t copy;
  
  copy = board->pieces[KNIGHTS] & board->colour[board->turn];
  while(copy)
  {
    from = __builtin_ctzll(copy);
    moves = magic_moves_knight(from) & allowed;
    
    while(moves)
    {
      to = __builtin_ctzll(moves);
      move_list[num_moves] = add_movecapture(board, from, to, KNIGHTS);
      num_moves++;
      moves &= moves-1;
    }
    copy &= copy-1;
  }
  
  return num_moves;
}

int find_moves_bishops(s_board *board, s_move *move_list, uint64_t allowed)
{
  assert(board != NULL);
  assert(move_list != NULL);
  
  int num_moves = 0;
  uint64_t moves;
  int from;
  int to;
  uint64_t copy;
  
  // Bishops
  copy = board->pieces[BISHOPS] & board->colour[board->turn];
  while(copy)
  {
    from = __builtin_ctzll(copy);
    moves = magic_moves_bishop((board->colour[WHITE]|board->colour[BLACK]), from) & allowed;
    
    while(moves)
    {
      to = __builtin_ctzll(moves);
      move_list[num_moves] = add_movecapture(board, from, to, BISHOPS);
      num_moves++;
      moves &= moves-1;
    }
    copy &= copy-1;
  }
  
  return num_moves;
}

int find_moves_rooks(s_board *board, s_move *move_list, uint64_t allowed)
{
  assert(board != NULL);
  assert(move_list != NULL);
  
  int num_moves = 0;
  uint64_t moves;
  int from;
  int to;
  uint64_t copy;
  
  // Rook
  copy = board->pieces[ROOKS] & board->colour[board->turn];
  while(copy)
  {
    from = __builtin_ctzll(copy);
    moves = magic_moves_rook((board->colour[WHITE]|board->colour[BLACK]), from) & allowed;
    
    while(moves)
    {
      to = __builtin_ctzll(moves);
      move_list[num_moves] = add_movecapture(board, from, to, ROOKS);
      num_moves++;
      moves &= moves-1;
    }
    copy &= copy-1;
  }
  
  return num_moves;
}

int find_moves_queens(s_board *board, s_move *move_list, uint64_t allowed)
{
  assert(board != NULL);
  assert(move_list != NULL);
  
  int num_moves = 0;
  uint64_t moves;
  int from;
  int to;
  uint64_t copy;
  
  // Queens (Bishop)
  copy = board->pieces[QUEENS] & board->colour[board->turn];
  while(copy)
  {
    from = __builtin_ctzll(copy);
    moves = magic_moves_bishop((board->colour[WHITE]|board->colour[BLACK]), from) & allowed;
    
    while(moves)
    {
      to = __builtin_ctzll(moves);
      move_list[num_moves] = add_movecapture(board, from, to, QUEENS);
      num_moves++;
      moves &= moves-1;
    }
    copy &= copy-1;
  }
  
  // Queens (Rook)
  copy = board->pieces[QUEENS] & board->colour[board->turn];
  while(copy)
  {
    from = __builtin_ctzll(copy);
    moves = magic_moves_rook((board->colour[WHITE]|board->colour[BLACK]), from) & allowed;
    
    while(moves)
    {
      to = __builtin_ctzll(moves);
      move_list[num_moves] = add_movecapture(board, from, to, QUEENS);
      num_moves++;
      moves &= moves-1;
    }
    copy &= copy-1;
  }
  
  return num_moves;
}

int find_moves_bishops_queens(s_board *board, s_move *move_list, uint64_t allowed)
{
  assert(board != NULL);
  assert(move_list != NULL);
  
  int num_moves = 0;
  uint64_t moves;
  int from;
  int to;
  uint64_t copy;
  
  // Bishops
  copy = board->pieces[BISHOPS] & board->colour[board->turn];
  while(copy)
  {
    from = __builtin_ctzll(copy);
    moves = magic_moves_bishop((board->colour[WHITE]|board->colour[BLACK]), from) & allowed;
    
    while(moves)
    {
      to = __builtin_ctzll(moves);
      move_list[num_moves] = add_movecapture(board, from, to, BISHOPS);
      num_moves++;
      moves &= moves-1;
    }
    copy &= copy-1;
  }
  
  // Queens (Bishop)
  copy = board->pieces[QUEENS] & board->colour[board->turn];
  while(copy)
  {
    from = __builtin_ctzll(copy);
    moves = magic_moves_bishop((board->colour[WHITE]|board->colour[BLACK]), from) & allowed;
    
    while(moves)
    {
      to = __builtin_ctzll(moves);
      move_list[num_moves] = add_movecapture(board, from, to, QUEENS);
      num_moves++;
      moves &= moves-1;
    }
    copy &= copy-1;
  }
  
  return num_moves;
}

int find_moves_rooks_queens(s_board *board, s_move *move_list, uint64_t allowed)
{
  assert(board != NULL);
  assert(move_list != NULL);
  
  int num_moves = 0;
  uint64_t moves;
  int from;
  int to;
  uint64_t copy;
  
  // Rook
  copy = board->pieces[ROOKS] & board->colour[board->turn];
  while(copy)
  {
    from = __builtin_ctzll(copy);
    moves = magic_moves_rook((board->colour[WHITE]|board->colour[BLACK]), from) & allowed;
    
    while(moves)
    {
      to = __builtin_ctzll(moves);
      move_list[num_moves] = add_movecapture(board, from, to, ROOKS);
      num_moves++;
      moves &= moves-1;
    }
    copy &= copy-1;
  }
  
  // Queens (Rook)
  copy = board->pieces[QUEENS] & board->colour[board->turn];
  while(copy)
  {
    from = __builtin_ctzll(copy);
    moves = magic_moves_rook((board->colour[WHITE]|board->colour[BLACK]), from) & allowed;
    
    while(moves)
    {
      to = __builtin_ctzll(moves);
      move_list[num_moves] = add_movecapture(board, from, to, QUEENS);
      num_moves++;
      moves &= moves-1;
    }
    copy &= copy-1;
  }
  
  return num_moves;
}

int find_moves_captures(s_board *board, s_move *move_list, int colour)
{
  assert(board != NULL);
  assert(move_list != NULL);
  assert(colour == WHITE || colour == BLACK);
  
  uint64_t allowed = board->colour[1-board->turn];
  
  int num_moves = 0;
  
  #ifdef GENERATE_SORTED
    // MVV-LVA (Most Valuable Victim - Least Valuable Aggressor)
    num_moves += find_moves_pawn_captures(board, &move_list[num_moves], allowed&board->pieces[QUEENS]);
    num_moves += find_moves_knights(board, &move_list[num_moves], allowed&board->pieces[QUEENS]);
    num_moves += find_moves_bishops(board, &move_list[num_moves], allowed&board->pieces[QUEENS]);
    num_moves += find_moves_rooks(board, &move_list[num_moves], allowed&board->pieces[QUEENS]);
    num_moves += find_moves_queens(board, &move_list[num_moves], allowed&board->pieces[QUEENS]);
    num_moves += find_moves_kings(board, &move_list[num_moves], allowed&board->pieces[QUEENS]);
    
    num_moves += find_moves_pawn_captures(board, &move_list[num_moves], allowed&board->pieces[ROOKS]);
    num_moves += find_moves_knights(board, &move_list[num_moves], allowed&board->pieces[ROOKS]);
    num_moves += find_moves_bishops(board, &move_list[num_moves], allowed&board->pieces[ROOKS]);
    num_moves += find_moves_rooks(board, &move_list[num_moves], allowed&board->pieces[ROOKS]);
    num_moves += find_moves_queens(board, &move_list[num_moves], allowed&board->pieces[ROOKS]);
    num_moves += find_moves_kings(board, &move_list[num_moves], allowed&board->pieces[ROOKS]);
    
    num_moves += find_moves_pawn_captures(board, &move_list[num_moves], allowed&board->pieces[BISHOPS]);
    num_moves += find_moves_knights(board, &move_list[num_moves], allowed&board->pieces[BISHOPS]);
    num_moves += find_moves_bishops(board, &move_list[num_moves], allowed&board->pieces[BISHOPS]);
    num_moves += find_moves_rooks(board, &move_list[num_moves], allowed&board->pieces[BISHOPS]);
    num_moves += find_moves_queens(board, &move_list[num_moves], allowed&board->pieces[BISHOPS]);
    num_moves += find_moves_kings(board, &move_list[num_moves], allowed&board->pieces[BISHOPS]);
    
    num_moves += find_moves_pawn_captures(board, &move_list[num_moves], allowed&board->pieces[KNIGHTS]);
    num_moves += find_moves_knights(board, &move_list[num_moves], allowed&board->pieces[KNIGHTS]);
    num_moves += find_moves_bishops(board, &move_list[num_moves], allowed&board->pieces[KNIGHTS]);
    num_moves += find_moves_rooks(board, &move_list[num_moves], allowed&board->pieces[KNIGHTS]);
    num_moves += find_moves_queens(board, &move_list[num_moves], allowed&board->pieces[KNIGHTS]);
    num_moves += find_moves_kings(board, &move_list[num_moves], allowed&board->pieces[KNIGHTS]);
    
    num_moves += find_moves_pawn_captures(board, &move_list[num_moves], allowed&board->pieces[PAWNS]);
    num_moves += find_moves_pawn_ep(board, &move_list[num_moves]);
    num_moves += find_moves_knights(board, &move_list[num_moves], allowed&board->pieces[PAWNS]);
    num_moves += find_moves_bishops(board, &move_list[num_moves], allowed&board->pieces[PAWNS]);
    num_moves += find_moves_rooks(board, &move_list[num_moves], allowed&board->pieces[PAWNS]);
    num_moves += find_moves_queens(board, &move_list[num_moves], allowed&board->pieces[PAWNS]);
    num_moves += find_moves_kings(board, &move_list[num_moves], allowed&board->pieces[PAWNS]);
  #else
    num_moves += find_moves_bishops_queens(board, &move_list[num_moves], allowed);
    num_moves += find_moves_rooks_queens(board, &move_list[num_moves], allowed);
    num_moves += find_moves_knights(board, &move_list[num_moves], allowed);
    num_moves += find_moves_pawn_captures(board, &move_list[num_moves], allowed);
    num_moves += find_moves_pawn_ep(board, &move_list[num_moves]);
    num_moves += find_moves_kings(board, &move_list[num_moves], allowed);
  #endif
  
  #ifndef NDEBUG
    int i;
    for(i = 0; i < num_moves; ++i)
    {
      assert(move_is_legal(board, &move_list[i]));
    }
  #endif
  
  return num_moves;
}

int find_moves_quiet(s_board *board, s_move *move_list, int colour)
{
  assert(board != NULL);
  assert(move_list != NULL);
  assert(colour == WHITE || colour == BLACK);
  
  uint64_t allowed = ~(board->colour[board->turn]|board->colour[!board->turn]);
  
  int num_moves = 0;
  
  num_moves += find_moves_bishops_queens(board, &move_list[num_moves], allowed);
  num_moves += find_moves_rooks_queens(board, &move_list[num_moves], allowed);
  num_moves += find_moves_knights(board, &move_list[num_moves], allowed);
  
  if(colour == WHITE)
  {
    num_moves += find_moves_wP_quiet(board, &move_list[num_moves]);
  }
  else
  {
    num_moves += find_moves_bP_quiet(board, &move_list[num_moves]);
  }
  
  num_moves += find_moves_kings(board, &move_list[num_moves], allowed);
  num_moves += find_moves_kings_castles(board, &move_list[num_moves]);
  
  #ifndef NDEBUG
    int i;
    for(i = 0; i < num_moves; ++i)
    {
      assert(move_is_legal(board, &move_list[i]));
    }
  #endif
  
  return num_moves;
}
