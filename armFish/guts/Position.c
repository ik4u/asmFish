
Position_SetState:
/*
	; in:  rbp  address of Pos
	; set information in state struct

	       push   rbx rsi rdi r12 r13 r14 r15
		sub   rsp, 64
		mov   rbx, qword[rbp+Pos.state]
*/
/*
		mov   rax, qword[Zobrist_side]
		mov   r15d, dword[rbp+Pos.sideToMove]
	      movzx   ecx, byte[rbx+State.epSquare]
	      movzx   edx, byte[rbx+State.castlingRights]
		neg   r15
		and   r15, qword[Zobrist_side]
		xor   r15, qword[Zobrist_Castling+8*rdx]
		cmp   ecx, 64
		jae   @f
		and   ecx, 7
		xor   r15, qword[Zobrist_Ep+8*rcx]
		@@:
*/
/*
		mov   r14, [Zobrist_noPawns]
		xor   r13, r13

	      vpxor   xmm0, xmm0, xmm0	; npMaterial
	    vmovdqa   dqword[rsp], xmm0

		xor   esi, esi
*/
Position_SetState.NextSquare:
/*
	      movzx   eax, byte[rbp+Pos.board+rsi]
		mov   edx, eax
		and   edx, 7	; edx = piece type
		 jz   .Empty
*/
/*
	       imul   ecx, eax, 64*8
	      vmovq   xmm1, qword[Scores_Pieces+rcx+8*rsi]
	     vpaddd   xmm0, xmm0, xmm1
*/
/*
		xor   r15, qword[Zobrist_Pieces+rcx+8*rsi]
		cmp   edx, Pawn
		jne   @f
		xor   r14, qword[Zobrist_Pieces+rcx+8*rsi]
	 @@:
*/
/*
	      movzx   edx, byte [rsp+rax]
		xor   r13, qword[Zobrist_Pieces+rcx+8*rdx]
		add   edx, 1
		mov   byte [rsp+rax], dl
*/
Position_SetState.Empty:
/*
		add   esi, 1
		cmp   esi, 64
		 jb   .NextSquare

		mov   qword[rbx+State.key], r15
		mov   qword[rbx+State.pawnKey], r14
		mov   qword[rbx+State.materialKey], r13
	      vmovq   qword[rbx+State.psq], xmm0
*/
/*
		mov   ecx, dword [rbp+Pos.sideToMove]
		mov   rdx, qword [rbp+Pos.typeBB+8*King]
		and   rdx, qword [rbp+Pos.typeBB+8*rcx]
		bsf   rdx, rdx
	       call   AttackersTo_Side
		mov   qword[rbx+State.checkersBB], rax

	       call   SetCheckInfo

		add   rsp, 64
		pop   r15 r14 r13 r12 rdi rsi rbx
		ret
*/



Position_SetPieceLists:
        ret
/*
	; in: rbp Position
	; out: set index, pieceCount, pieceList members in some fixed order
	       push   rbx rsi rdi

	; fill indices with invalid index 0
		lea   rdi, [rbp+Pos.pieceIdx]
		xor   eax, eax
		mov   ecx, 64
	  rep stosb
*/
/*
	; fill piece counts with indices indicating no pieces on the board
irps c, White Black {
		mov   byte[rbp+Pos.pieceEnd+(8*c+0)]	 , 0
		mov   byte[rbp+Pos.pieceEnd+(8*c+1)]	 , 0
		mov   byte[rbp+Pos.pieceEnd+(8*c+Pawn)]  , 16*(8*c+Pawn)
		mov   byte[rbp+Pos.pieceEnd+(8*c+Knight)], 16*(8*c+Knight)
		mov   byte[rbp+Pos.pieceEnd+(8*c+Bishop)], 16*(8*c+Bishop)
		mov   byte[rbp+Pos.pieceEnd+(8*c+Rook)]  , 16*(8*c+Rook)
		mov   byte[rbp+Pos.pieceEnd+(8*c+Queen)] , 16*(8*c+Queen)
		mov   byte[rbp+Pos.pieceEnd+(8*c+King)]  , 16*(8*c+King)
}
*/
/*
	; fill piece lists with SQ_NONE
		lea   rdi, [rbp+Pos.pieceList]
		mov   eax, 64
		mov   ecx, 16*16
	  rep stosb

	; the order is A8 to H8, then A7 to H7, ect
		xor   esi, esi
*/
Position_SetPieceLists.NextSquare:
/*
		xor   esi, 56
	      movzx   eax, byte[rbp+Pos.board+rsi]
	       test   eax, eax
		 jz   .skip
	      movzx   ecx, byte[rbp+Pos.pieceEnd+rax]
		mov   byte[rbp+Pos.pieceIdx+rsi], cl
		mov   byte[rbp+Pos.pieceList+rcx], sil
		add   ecx, 1
		mov   byte[rbp+Pos.pieceEnd+rax], cl
*/
Position_SetPieceLists.skip:
/*
		xor   esi, 56
		add   esi, 1
		cmp   esi, 64
		 jb   .NextSquare
*/
Position_SetPieceLists.Done:
/*
		pop   rdi rsi rbx
		ret
*/





Position_IsLegal:
        mov  x0, 0
        ret
/*
	; in: rbp position
	; out: eax =  0 if position is legal more checks are performed with DEBUG
	;      eax = -1 if position is illegal
	;      rdx address of string

	       push   rbx rdi

	; pieces should not intersect
		mov   rax, qword[rbp+Pos.typeBB+8*White]
		mov   rcx, qword[rbp+Pos.typeBB+8*Black]
	       test   rax, rcx
		jnz   .Failed

	; at most 16 of each type
	     popcnt   rax, rax, r8
		cmp   eax, 17
		jae   .Failed
	     popcnt   rcx, rcx, r8
		cmp   ecx, 17
		jae   .Failed

	; at most 2 checkers
		mov   rbx, qword[rbp+Pos.state]
	     popcnt   rax, qword[rbx+State.checkersBB], r8
		cmp   eax, 3
		jae   .Failed
*/
Position_IsLegal.VerifyKings:
/*
		mov   rax, qword[rbp+Pos.typeBB+8*White]
		and   rax, qword[rbp+Pos.typeBB+8*King]
	     popcnt   rax, rax, r8
		cmp   eax, 1
		jne   .Failed
		mov   rax, qword[rbp+Pos.typeBB+8*Black]
		and   rax, qword[rbp+Pos.typeBB+8*King]
	     popcnt   rax, rax, r8
		cmp   eax, 1
		jne   .Failed
*/
Position_IsLegal.VerifyPawns:
/*
		mov   rax, 0xFF000000000000FF
	       test   rax, qword[rbp+Pos.typeBB+8*Pawn]
		jnz   .Failed
*/
Position_IsLegal.VerifyPieces:
/*
		mov   rcx, qword[rbp+Pos.typeBB+8*White]
		mov   r9, rcx
		and   rcx, qword[rbp+Pos.typeBB+8*King]
	     popcnt   rdx, r9, r8
irps p, Pawn Knight Bishop Rook Queen {
		mov   rax, qword[rbp+Pos.typeBB+8*p]
		and   rax, r9
		 or   rcx, rax
	     popcnt   rax, rax, r8
		sub   edx, eax
}
		sub   edx, 1
		jnz   .Failed
		cmp   rcx, r9
		jne   .Failed
*/
/*
		mov   rcx, qword[rbp+Pos.typeBB+8*Black]
		mov   r9, rcx
		and   rcx, qword[rbp+Pos.typeBB+8*King]
	     popcnt   rdx, r9, r8
irps p, Pawn Knight Bishop Rook Queen {
		mov   rax, qword[rbp+Pos.typeBB+8*p]
		and   rax, r9
		 or   rcx, rax
	     popcnt   rax, rax, r8
		sub   edx, eax
}
		sub   edx, 1
		jnz   .Failed
		cmp   rcx, r9
		jne   .Failed

		xor   edx, edx
*/

Position_IsLegal.VerifyBoard:
/*
	      movzx   eax, byte[rbp+Pos.board+rdx]
	       test   eax, eax
		 jz   .empty
		cmp   eax, 16
		jae   .Failed
		mov   ecx, eax
		and   eax, 7
		 jz   .Failed
		cmp   eax, 1
		 je   .Failed
		and   ecx, 8
		mov   r8, qword[rbp+Pos.typeBB+8*rax]
		and   r8, qword[rbp+Pos.typeBB+rcx]
		 bt   r8, rdx
		jnc   .Failed
		jmp   .next
*/
Position_IsLegal.empty:
/*
		mov   r8, qword[rbp+Pos.typeBB+8*0]
		 or   r8, qword[rbp+Pos.typeBB+8*1]
		 or   r8, qword[rbp+Pos.typeBB+8*2]
		 or   r8, qword[rbp+Pos.typeBB+8*3]
		 or   r8, qword[rbp+Pos.typeBB+8*4]
		 or   r8, qword[rbp+Pos.typeBB+8*5]
		 or   r8, qword[rbp+Pos.typeBB+8*6]
		 or   r8, qword[rbp+Pos.typeBB+8*7]
		 bt   r8, rdx
		 jc   .Failed
*/
Position_IsLegal.next:
/*
		add   edx, 1
		cmp   edx, 64
		 jb   .VerifyBoard
*/
Position_IsLegal.VerifyEp:
/*
	      movzx   ecx, byte [rbx+State.epSquare]
		cmp   ecx, 64
		jae   .VerifyEpDone
		mov   rax, Rank3BB+Rank6BB
		 bt   rax, rcx
		jnc  .Failed
	; make sure square behind ep square is empty
	      movzx   eax, byte[rbp+Pos.sideToMove]
		xor   eax, 1
		mov   rdx, qword[rbp+Pos.typeBB+8*rax]
		shl   eax, 4
		lea   eax, [rax+rcx-8]
		 bt   qword[rbp+Pos.typeBB+8*Black], rax
		 jc   .Failed
		 bt   qword[rbp+Pos.typeBB+8*White], rax
		 jc   .Failed
	; and square in front is occupied by one of their pawns
	      movzx   eax, byte[rbp+Pos.sideToMove]
		and   rdx, qword[rbp+Pos.typeBB+8*Pawn]
		shl   eax, 4
		lea   eax, [rax+rcx-8]
		 bt   rdx, rax
		jnc   .Failed
	; and opposing pawn can capture ep square
	      movzx   eax, byte[rbp+Pos.sideToMove]
		mov   rdx, qword[rbp+Pos.typeBB+8*rax]
		and   rdx, qword[rbp+Pos.typeBB+8*Pawn]
		xor   eax, 1
		shl   eax, 6+3
	       test   rdx, qword[WhitePawnAttacks+rax+8*rcx]
		 jz   .Failed
*/
Position_IsLegal.VerifyEpDone:


Position_IsLegal.VerifyKingCapture:
/*
	; make sure we can't capture their king
	      movzx   ecx, byte[rbp+Pos.sideToMove]
		xor   ecx, 1
		mov   rdx, qword[rbp+Pos.typeBB+8*King]
		and   rdx, qword[rbp+Pos.typeBB+8*rcx]
		bsf   rdx, rdx
	       call   AttackersTo_Side
	       test   rax, rax
		jnz   .Failed

		xor   eax, eax
		pop   rdi rbx
		ret
*/
Position_IsLegal.Failed:
/*
		 or   eax, -1
		pop   rdi rbx
		ret
*/




Position_ParseFen:
/*
	; in: rsi address of fen string
	;     rbp address of Pos
	;     ecx isChess960
	; out: eax = 0 success
	;      eax = -1 failure

	       push   rbp rbx rdi r12 r13 r14 r15
		mov   r12d, ecx

		mov   rbx, qword[rbp+Pos.stateTable]
	       test   rbx, rbx
		 jz   .alloc
*/
        stp  x29, x30, [sp, -16]!
        stp  x27, x21, [sp, -16]!
        stp  x22, x23, [sp, -16]!
        stp  x24, x25, [sp, -16]!
        mov  w22, w1
        ldr  x21, [x20, Pos.stateTable]
        cbz  x21, Position_ParseFen.alloc


Position_ParseFen.alloc_ret:
/*
		xor   eax, eax
		mov   ecx, Pos._copy_size/8
		mov   rdi, rbp
	  rep stosq
		mov   dword[rbp+Pos.chess960], r12d
*/
        mov  x0, x20
        mov  x1, 0
        mov  x2, Pos._copy_size
         bl  MemoryFill
/*
		xor   eax, eax
		mov   ecx, sizeof.State/8
		mov   rdi, rbx
	  rep stosq
*/
        mov  x0, x20
        mov  x1, 0
        mov  x2, sizeof.State
         bl  MemoryFill
/*
		xor   eax, eax
		mov   ecx, Thread.castling_end-Thread.castling_start
		lea   rdi, [rbp-Thread.rootPos+Thread.castling_start]
	  rep stosb

	       call   SkipSpaces

		xor   eax,eax
		xor   ecx,ecx
		jmp   .ExpectPiece
*/
        add  x0, x21, -Thread.rootPos+Thread.castling_start
        mov  x1, 0
        mov  x2, Thread.castling_end-Thread.castling_start
         bl  MemoryFill
         bl  SkipSpaces
        mov  x0, 0
        mov  x1, 0
          b  Position_ParseFen.ExpectPiece

Position_ParseFen.ExpectPieceOrSlash:
/*
	       test   ecx,7
		jnz   .ExpectPiece
	      lodsb
		cmp   al, '/'
		jne   .Failed
*/
        tst  x1, 7
        bne  Position_ParseFen.ExpectPiece
       ldrb  w0, [x26], 1
        cmp  w0, '/'
        bne  Position_ParseFen.Failed

Position_ParseFen.ExpectPiece:
/*
	      lodsb

		mov   edx, 8*White+Pawn
		cmp   al, 'P'
		 je   .FoundPiece
		mov   edx, 8*White+Knight
		cmp   al, 'N'
		 je   .FoundPiece
		mov   edx, 8*White+Bishop
		cmp   al, 'B'
		 je   .FoundPiece
		mov   edx, 8*White+Rook
		cmp   al, 'R'
		 je   .FoundPiece
		mov   edx, 8*White+Queen
		cmp   al, 'Q'
		 je   .FoundPiece
		mov   edx, 8*White+King
		cmp   al, 'K'
		 je   .FoundPiece

		mov   edx, 8*Black+Pawn
		cmp   al, 'p'
		 je   .FoundPiece
		mov   edx, 8*Black+Knight
		cmp   al, 'n'
		 je   .FoundPiece
		mov   edx, 8*Black+Bishop
		cmp   al, 'b'
		 je   .FoundPiece
		mov   edx, 8*Black+Rook
		cmp   al, 'r'
		 je   .FoundPiece
		mov   edx, 8*Black+Queen
		cmp   al, 'q'
		 je   .FoundPiece
		mov   edx, 8*Black+King
		cmp   al, 'k'
		 je   .FoundPiece

		sub   eax, '0'
		 js   .Failed
		cmp   eax, 8
		 ja   .Failed
*/
       ldrb  w0, [x26], 1

        mov  x2, 8*White+Pawn
        cmp  w0, 'P'
        beq  Position_ParseFen.FoundPiece
        mov  x2, 8*White+Knight
        cmp  w0, 'N'
        beq  Position_ParseFen.FoundPiece
        mov  x2, 8*White+Bishop
        cmp  w0, 'B'
        beq  Position_ParseFen.FoundPiece
        mov  x2, 8*White+Rook
        cmp  w0, 'R'
        beq  Position_ParseFen.FoundPiece
        mov  x2, 8*White+Queen
        cmp  w0, 'Q'
        beq  Position_ParseFen.FoundPiece
        mov  x2, 8*White+King
        cmp  w0, 'K'
        beq  Position_ParseFen.FoundPiece

        mov  x2, 8*Black+Pawn
        cmp  w0, 'p'
        beq  Position_ParseFen.FoundPiece
        mov  x2, 8*Black+Knight
        cmp  w0, 'n'
        beq  Position_ParseFen.FoundPiece
        mov  x2, 8*Black+Bishop
        cmp  w0, 'b'
        beq  Position_ParseFen.FoundPiece
        mov  x2, 8*Black+Rook
        cmp  w0, 'r'
        beq  Position_ParseFen.FoundPiece
        mov  x2, 8*Black+Queen
        cmp  w0, 'q'
        beq  Position_ParseFen.FoundPiece
        mov  x2, 8*Black+King
        cmp  w0, 'k'
        beq  Position_ParseFen.FoundPiece

       subs  x0, x0, '0'
        bmi  Position_ParseFen.Failed
        cmp  x0, 8
        bhi  Position_ParseFen.Failed
        
Position_ParseFen.Spaces:
/*
		add   ecx, eax
		jmp   .PieceDone
*/
        add  x1, x1, x0
          b  Position_ParseFen.PieceDone

Position_ParseFen.FoundPiece:
/*
		xor   ecx, 0111000b
		mov   edi, edx
		and   edi, 7
		bts   qword[rbp+Pos.typeBB+8*rdi], rcx
		mov   edi, edx
		shr   edi, 3
		bts   qword[rbp+Pos.typeBB+8*rdi], rcx
		mov   byte[rbp+Pos.board+rcx], dl
		xor   ecx, 0111000b
		add   ecx, 1
*/
        eor  x3, x1, 56
        add  x1, x1, 1
        mov  x7, 1
        lsl  x7, x7, x3
        and  x4, x2, 7
        lsr  x5, x2, 3
        ldr  x0, [x20, x4, lsl 3]
        orr  x0, x0, x7
        str  x0, [x20, x4, lsl 3]
        ldr  x0, [x20, x5, lsl 3]
        orr  x0, x0, x7
        str  x0, [x20, x5, lsl 3]
        add  x16, x20, Pos.board
       strb  w2, [x16, x3]

Position_ParseFen.PieceDone:
/*
		cmp   ecx, 64
		 jb   .ExpectPieceOrSlash
*/
        tbz  x1, 6, Position_ParseFen.ExpectPieceOrSlash

Position_ParseFen.Turn:
/*
	       call   SkipSpaces
	      lodsb
		xor   ecx, ecx
		cmp   al, 'b'
	       sete   cl
		mov   dword[rbp+Pos.sideToMove], ecx
      .Castling:
	       call   SkipSpaces
		xor   eax, eax
	      lodsb
		cmp   al, '-'
		 je   .EpSquare
*/
         bl  SkipSpaces
       ldrb  w0, [x26], 1
        cmp  w0, 'b'
       cset  w1, eq
        str  w1, [x20, Pos.sideToMove]

         bl  SkipSpaces
       ldrb  w0, [x26], 1
        cmp  w0, '-'
        beq  Position_ParseFen.EpSquare

Position_ParseFen.NextCastlingChar:
/*
		mov   edx, 1
		mov   ecx, eax
		sub   eax, 'A'
		cmp   eax, 'Z'-'A' + 1
		jae   .Lower
		add   ecx, ('a'-'A')
		sub   edx, 1
	.Lower:
	       call   SetCastlingRights
	       test   eax, eax
		jnz   .Failed
		xor   eax, eax
	      lodsb
		cmp   al, ' '
		jne   .NextCastlingChar
*/
        cmp  x0, 'Z'
        orr  x1, x0, 0x020
       cset  x2, hi
         bl  SetCastlingRights
       cbnz  w0, Position_ParseFen.Failed
       ldrb  w0, [x26], 1
        cmp  w0, ' '
        bne  Position_ParseFen.NextCastlingChar

Position_ParseFen.EpSquare:
/*
	       call   SkipSpaces
	       call   ParseSquare
		mov   byte[rbx+State.epSquare], al
		cmp   eax, 64
		 je   .FiftyMoves
		 ja   .Failed

		mov   edx, dword[rbp+Pos.sideToMove]
		mov   r9, qword[rbp+Pos.typeBB+8*rdx]	; r9 = our pieces
		xor   edx, 1

	; make sure ep square is on our 6th rank
		lea   ecx, [RANK_3+(RANK_6-RANK_3)*rdx]
		 bt   qword[RankBB+8*rcx], rax
		jnc   .EpSquareBad

	; make sure ep square and square above is empty
		mov   r8, qword[rbp+Pos.typeBB+8*rdx]
		mov   r10, r8				; r10 = their pieces
		 or   r8, r9
		 bt   r8, rax
		 jc   .EpSquareBad
		lea   ecx, [8*rdx-4]
		lea   rcx, [rax+2*rcx]
		 bt   r8, rcx
		 jc   .EpSquareBad

	; make sure our pawn is in position to attack ep square
		mov   ecx, edx
		shl   ecx, 6+3
		and   r9, qword[rbp+Pos.typeBB+8*Pawn]
	       test   r9, qword[PawnAttacks+rcx+8*rax]
		 jz   .EpSquareBad    

	; make sure square below has their pawn
		and   r10, qword[rbp+Pos.typeBB+8*Pawn]
		xor   edx, 1
		lea   ecx, [8*rdx-4]
		lea   rcx, [rax+2*rcx]
		 bt   r10, rcx
		jnc   .EpSquareBad

		jmp   .FiftyMoves
*/
         bl  SkipSpaces
         bl  ParseSquare
       strb  w0, [x21, State.epSquare]
        cmp  w0, 64
        beq  Position_ParseFen.FiftyMoves
        bhi  Position_ParseFen.Failed
        
          b  Position_ParseFen.FiftyMoves
        

Position_ParseFen.EpSquareBad:
/*
	; we can either fail here or set it to SQ_NONE
		mov   byte[rbx+State.epSquare], 64
*/
        mov  w0, 64
       strb  w0, [x21, State.epSquare]

Position_ParseFen.FiftyMoves:
/*
	       call   SkipSpaces
	       call   ParseInteger
		mov   word[rbx+State.rule50], ax
*/
        bl  SkipSpaces
        bl  ParseInteger
      strh  w0, [x21, State.rule50]

Position_ParseFen.MoveNumber:
/*
	       call   SkipSpaces
	       call   ParseInteger
		sub   eax, 1
		adc   eax, 0
		add   eax, eax
		add   eax, dword[rbp+Pos.sideToMove]
		mov   dword[rbp+Pos.gamePly], eax

	       call   Position_SetState
	       call   Position_SetPieceLists
	       call   Position_IsLegal
	       test   eax,eax
		jnz   .Failed
*/
         bl  SkipSpaces
         bl  ParseInteger
       subs  x1, x0, 1
       csel  x0, xzr, x1, mi
        ldr  w2, [x20, Pos.sideToMove]
        add  x0, x2, x0, lsl 1
        str  w0, [x20, Pos.gamePly]

         bl  Position_SetState
         bl  Position_SetPieceLists
         bl  Position_IsLegal
       cbnz  w0, Position_ParseFen.Failed

Position_ParseFen.done:
        ldp  x24, x25, [sp], 16
        ldp  x22, x23, [sp], 16
        ldp  x27, x21, [sp], 16
        ldp  x29, x30, [sp], 16
        ret

Position_ParseFen.Failed:
        mov  w0, -1
          b  Position_ParseFen.done

Position_ParseFen.alloc:
        mov  w1, 64*sizeof.State
        mov  w25, w1
         bl  Os_VirtualAlloc
        mov  x21, x0
        str  x0, [x20, Pos.state]
        str  x0, [x20, Pos.stateTable]
        add  x0, x0, x25
        str  x0, [x20, Pos.stateEnd]
          b  Position_ParseFen.alloc_ret









SetCastlingRights:
/*
	; in: edx color
	;     ecx = 'q' for qeenside castling
	;           'k' for kingside castling
	;           'a' through 'h' for file of rook
	;     rbp position
	;     rbx state
	; out eax = 0 if success
	;         = -1 if failed

	       push   rdi rsi r12 r13 r14 rsi

		mov   rdi, qword[rbp+Pos.typeBB+8*King]
		and   rdi, qword[rbp+Pos.typeBB+8*rdx]
		bsf   rdi, rdi
		 jz   .failed
	       imul   esi, edx, 56
		lea   r8d, [8*rdx+Rook]
		add   esi, 7
		 or   r9d, -1
		cmp   cl, 'k'
		 je   .find_rook_sq
		sub   esi, 7
		neg   r9d
		cmp   cl, 'q'
		 je   .find_rook_sq
		sub   ecx, 'a'
		add   esi, ecx
		cmp   ecx, 7
		 ja   .failed
*/
        ldr  x7, [x20, 8*King]
        ldr  x4, [x20, x2, lsl 3]
        and  x7, x7, x4
       rbit  x4, x7
        cbz  x7, SetCastlingRights.failed
        clz  x7, x4
        mov  x4, 56
        mul  x6, x2, x4
        lsl  x8, x2, 3
        add  x8, x8, Rook
        add  x6, x6, 7
        mov  x9, -1
        cmp  x1, 'k'
        beq  SetCastlingRights.find_rook_sq
        sub  x6, x6, 7
        mov  x9, 1
        cmp  x1, 'q'
        beq  SetCastlingRights.find_rook_sq
        sub  x1, x1, 'a'
        add  x6, x6, x1
        cmp  x1, 7
        bhi  SetCastlingRights.failed
        
        

SetCastlingRights.have_rook_sq:
/*
	; esi = rook from
	; edi = king from
		cmp   r8l, byte[rbp+Pos.board+rsi]
		jne   .failed

	; r14 = 0 if OO, 1 if OOO
		xor   r14, r14
		cmp   esi, edi
		adc   r14, r14

	; r15 = 2*color + r14
		lea   r15, [2*rdx+r14]
*/
        add  x16, x20, Pos.board
       ldrb  w4, [x16, x6]
        cmp  x8, x4
        bne  SetCastlingRights.failed
        cmp  x6, x7
       cset  x14, lo
        add  x15, x14, x2, lsl 1
/*
	; r8 = rook to
	; r9 = king to
	      movzx   r8d, byte[rsquare_lookup+r15]
	      movzx   r9d, byte[ksquare_lookup+r15]

	; set castling rights
	      movzx   eax, byte[rbx+State.castlingRights]
		bts   eax, r15d
		mov   byte[rbx+State.castlingRights], al

	; set masks
	      movzx   eax, byte[rbp-Thread.rootPos+Thread.castling_rightsMask+rsi]
		bts   eax, r15d
		mov   byte[rbp-Thread.rootPos+Thread.castling_rightsMask+rsi], al
	      movzx   eax, byte[rbp-Thread.rootPos+Thread.castling_rightsMask+rdi]
		bts   eax, r15d
		mov   byte[rbp-Thread.rootPos+Thread.castling_rightsMask+rdi], al
*/

        adr  x16, SetCastlingRights.rsquare_lookup
        adr  x17, SetCastlingRights.ksquare_lookup
       ldrb  w8, [x16, x15]
       ldrb  w9, [x17, x15]
        add  x16, x20, -Thread.rootPos+Thread.castling_rightsMask
        mov  x3, 1
        lsl  x3, x3, x15
       ldrb  w0, [x21, State.castlingRights]
        orr  w0, w0, w3
       strb  w0, [x21, State.castlingRights]
       ldrb  w0, [x16, x6]
        orr  w0, w0, w3
       strb  w0, [x16, x6]
       ldrb  w0, [x16, x7]
        orr  w0, w0, w3
       strb  w0, [x16, x7]

/*
	; set rook from/to
		mov   byte[rbp-Thread.rootPos+Thread.castling_rfrom+r15], sil
		mov   byte[rbp-Thread.rootPos+Thread.castling_rto+r15], r8l

	; set castling path
		lea   r11, [rbp-Thread.rootPos+Thread.castling_ksqpath+8*r15]
		xor   eax, eax
		mov   qword[r11], rax
		mov   r12, rdi
		mov   r13, r9
		cmp   r12, r13
		 jb   @f
	       xchg   r12, r13
	@@:	sub   r12, 1
*/
        add  x16, x20, -Thread.rootPos+Thread.castling_rfrom
        add  x17, x20, -Thread.rootPos+Thread.castling_rto
       strb  w6, [x16, x15]
       strb  w8, [x17, x15]

        add  x11, x20, -Thread.rootPos+Thread.castling_ksqpath
        add  x11, x11, x15, lsl 3
        str  xzr, [x11]
        cmp  x7, x9
       csel  x12, x7, x9, lo
       csel  x13, x7, x9, hi
        sub  x12, x12, 1
        

SetCastlingRights.king_loop:
/*
		add   r12, 1
		cmp   r12, r13
		 ja   .king_loop_done
		cmp   r12, rdi
		 je   .king_loop
		add   byte[rbp-Thread.rootPos+Thread.castling_ksqpath+8*r15], 1
		add   r11, 1
		mov   byte[r11], r12l
		mov   rcx, qword[KnightAttacks+8*r12]
		 or   qword[rbp-Thread.rootPos+Thread.castling_knights+8*r15], rcx
		mov   rcx, qword[KingAttacks+8*r12]
		 or   qword[rbp-Thread.rootPos+Thread.castling_kingpawns+8*r15], rcx
		cmp   r12, rsi
		 je   .king_loop
		bts   rax, r12
		jmp   .king_loop
*/
        add  x12, x12, 1
        cmp  x12, x13
        bhi  SetCastlingRights.king_loop_done
        cmp  x12, x7
        beq  SetCastlingRights.king_loop

        add  x16, x20, -Thread.rootPos+Thread.castling_ksqpath
        ldr  x4, [x16, x15, lsl 3]
        add  x4, x4, 1
        str  x4, [x16, x15, lsl 3]

        add  x11, x11, 1
       strb  w12, [x11]

        lea  x16, KnightAttacks
        ldr  x1, [x16, x12, lsl 3]
        add  x16, x21, -Thread.rootPos+Thread.castling_knights
        ldr  x4, [x16, x15, lsl 3]
        orr  x4, x4, x1
        str  x4, [x16, x15, lsl 3]

        lea  x16, KingAttacks
        ldr  x1, [x16, x12, lsl 3]
        add  x16, x21, -Thread.rootPos+Thread.castling_kingpawns
        ldr  x4, [x16, x15, lsl 3]
        orr  x4, x4, x1
        str  x4, [x16, x15, lsl 3]

        cmp  x12, x6
        beq  SetCastlingRights.king_loop
        mov  x4, 1
        lsl  x4, x4, x12
        orr  x0, x0, x4
        cmp  x12, x6
          b  SetCastlingRights.king_loop


SetCastlingRights.king_loop_done:
/*
		mov   r12, rsi
		mov   r13, r8
		cmp   r12, r13
		 jb   @f
	       xchg   r12, r13
	@@:	sub   r12, 1
*/
        cmp  x6, x8
       csel  x12, x6, x8, lo
       csel  x13, x6, x8, hi
        sub  x12, x12, 1

SetCastlingRights.rook_loop:
/*
		add   r12, 1
		cmp   r12, r13
		 ja   .rook_loop_done
		cmp   r12, rdi
		 je   .rook_loop
		cmp   r12, rsi
		 je   .rook_loop
		bts   rax, r12
		jmp   .rook_loop
*/
        add  x12, x12, 1
        cmp  x12, x13
        bhi  SetCastlingRights.rook_loop_done
        cmp  x12, x7
        beq  SetCastlingRights.rook_loop
        cmp  x12, x6
        beq  SetCastlingRights.rook_loop
        mov  x4, 1
        lsl  x4, x4, x12
        orr  x0, x0, x4
          b  SetCastlingRights.rook_loop

SetCastlingRights.rook_loop_done:
/*
		mov   qword[rbp-Thread.rootPos+Thread.castling_path+8*r15], rax

	; set castling move
		mov   eax, MOVE_TYPE_CASTLE
		shl   eax, 6
		add   eax, edi
		shl   eax, 6
		add   eax, esi
		mov   dword[rbp-Thread.rootPos+Thread.castling_movgen+4*r15], eax

		xor   eax, eax
*/
        add  x16, x20, -Thread.rootPos+Thread.castling_path
        str  x0, [x16, x15, lsl 3]

        mov  w0, MOVE_TYPE_CASTLE
        add  w0, w7, w0, lsl 6
        add  w0, w6, w0, lsl 6
        add  x16, x20, -Thread.rootPos+Thread.castling_movgen
        str  w0, [x16, x15, lsl 2]

        mov  w0, 0

SetCastlingRights.done:
/*
		pop   r15 r14 r13 r12 rsi rdi
		ret
*/
        ret

SetCastlingRights.failed:
/*
		 or   eax, -1
		jmp   .done
*/
        mov  w0, -1
        ret

SetCastlingRights.find_rook_sq:
/*
		cmp   esi, 64
		jae   .failed
		cmp   r8l, byte[rbp+Pos.board+rsi]
		 je   .have_rook_sq
		add   esi, r9d
		jmp   .find_rook_sq
*/
        cmp  x6, 64
        bhs  SetCastlingRights.failed
        add  x16, x20, Pos.board
       ldrb  w4, [x16, x6]
        cmp  x8, x4
        beq  SetCastlingRights.have_rook_sq
        add  x6, x6, x9
          b  SetCastlingRights.find_rook_sq

SetCastlingRights.rsquare_lookup:
        .byte  SQ_F1, SQ_D1, SQ_F8, SQ_D8
SetCastlingRights.ksquare_lookup:
        .byte  SQ_G1, SQ_C1, SQ_G8, SQ_C8













Position_PrintFen:
/*
	; in: rbp address of Pos
	; io: rdi string

	       push   rbx
		mov   rbx, qword[rbp+Pos.state]

		mov   r8d, 7
*/
        stp  x21, x30, [sp, -16]!
        ldr  x21, [x20, Pos.state]


       ldrb  w1, [x21, State.castlingRights]
        mov  x8, 7
Position_PrintFen.loop1:
/*
		xor   ecx, ecx

		xor   r9d, r9d
*/
        mov  x1, 0
        mov  x9, 0

Position_PrintFen.loop2:
/*
		lea   r10d, [r9+8*r8]
	      movzx   edx, byte[rbp+Pos.board+r10]
	       test   edx, edx
		 jz   .space

		lea   eax, ['0'+rcx]
	       test   ecx,ecx
		 jz   @f
	      stosb
	@@:
	      movzx   eax, byte[PieceToChar+rdx]
	      stosb

		xor   ecx,ecx

		jmp   .cont
*/
        add  x10, x9, x8, lsl 3
        add  x16, x20, Pos.board
       ldrb  w2, [x16, x10]

        cbz  w2, Position_PrintFen.space
        add  x0, x1, '0'
        tst  x1, x1
       strb  w0, [x27]
       cinc  x27, x27, ne
        lea  x16, PieceToChar
       ldrb  w0, [x16, x2]
       strb  w0, [x27], 1
        mov  x1, 0
          b  Position_PrintFen.cont

Position_PrintFen.space:
/*
		add   ecx,1
*/
        add  x1, x1, 1

Position_PrintFen.cont:
/*
		add   r9d,1
		cmp   r9d,8
		 jb   .loop2

		lea   eax, ['0'+rcx]
	       test   ecx, ecx
		 jz   @f
	      stosb
	@@:
		mov   al, '/'
	      stosb

		sub   r8d, 1
		jns   .loop1
*/
        add  x9, x9, 1
        cmp  x9, 8
        blo  Position_PrintFen.loop2
        add  x0, x1, '0'
        tst  x1, x1
       strb  w0, [x27]
       cinc  x27, x27, ne
        mov  w0, '/'
       strb  w0, [x27], 1
       subs  x8, x8, 1
        bpl  Position_PrintFen.loop1
/*
	; side to move
		mov   byte[rdi-1], ' '
		mov   eax, 'w '
		mov   ecx, 'b '
		cmp   byte[rbp+Pos.sideToMove], 0
	     cmovne   eax, ecx
	      stosw

	; castling
	      movzx   ecx, byte[rbx+State.castlingRights]
		mov   byte[rdi], '-'
		cmp   ecx, 1
		adc   rdi, 0
		mov   eax, 'KQkq'
		mov   edx, dword[rbp-Thread.rootPos+Thread.castling_rfrom]
		and   edx, 0x07070707
		add   edx, 'AAaa'
		cmp   byte[rbp+Pos.chess960], 0
	     cmovne   eax, edx
	irps i, 0 1 2 3 {
		mov   byte[rdi], al
		shr   eax, 8
		 bt   ecx, i
		adc   rdi, 0
	}

	; ep
		mov   eax, ' '
	      stosb
	      movzx   rcx, byte[rbx+State.epSquare]
	       call   PrintSquare

	; 50 moves
		mov   eax, ' '
	      stosb
	      movzx   eax, word[rbx+State.rule50]
	       call   PrintUnsignedInteger

	; ply
		mov   eax, ' '
	      stosb

		mov   eax, dword[rbp+Pos.gamePly]
		add   eax, 2
		shr   eax, 1
	       call   PrintUnsignedInteger

		pop   rbx
		ret
*/
        mov  w0, ' '
       strb  w0, [x27, -1]
        mov  w0, ('w'<<0) + (' '<<8)
        mov  w1, ('b'<<0) + (' '<<8)
       strb  w4, [x20, Pos.sideToMove]
        tst  w4, w4
       csel  w0, w2, w0, ne
       strh  w0, [x27], 2

       ldrb  w1, [x21, State.castlingRights]
        mov  w0, '-'
       strb  w0, [x27]
        tst  w1, w1
       cinc  x27, x27, eq

        mov  w0, ('K'<<0) + ('Q'<<8)
       movk  w0, ('k'<<0) + ('q'<<8), lsl 16
        ldr  w2, [x20, -Thread.rootPos+Thread.castling_rfrom]
        mov  w4, 0x07070707
        and  w2, w2, w4
        mov  w4, ('A'<<0) + ('A'<<8)
       movk  w4, ('a'<<0) + ('a'<<8), lsl 16
        add  w2, w2, w4
        ldr  w4, [x21, Pos.chess960]
        tst  w4, w4
       csel  w0, w2, w0, ne

       strb  w0, [x27]
        lsr  w0, w0, 8
        tst  x1, 1
       cinc  x27, x27, ne
       strb  w0, [x27]
        lsr  w0, w0, 8
        tst  x1, 2
       cinc  x27, x27, ne
       strb  w0, [x27]
        lsr  w0, w0, 8
        tst  x1, 4
       cinc  x27, x27, ne
       strb  w0, [x27]
        lsr  w0, w0, 8
        tst  x1, 8
       cinc  x27, x27, ne

        mov  w0, ' '
       strb  w0, [x27], 1
       ldrb  w1, [x21, State.epSquare]
         bl  PrintSquare

        mov  w0, ' '
       strb  w0, [x27], 1
       ldrh  w0, [x21, State.rule50]
         bl  PrintUInt

        mov  w0, ' '
       strb  w0, [x27], 1
        ldr  w0, [x21, Pos.gamePly]
        add  w0, w0, 2
        lsr  w0, w0, 1
         bl  PrintUInt

        ldp  x21, x30, [sp], 16
        ret



Position_CopyTo:
/*
	; rbp address of source position
	; rcx address of destination position
	;  up to Pos._copy_size is copied
	;  and state array is copied

	       push   rbx rsi rdi r13 r14
		mov   r13, rcx

	; copy castling data
		mov   ecx, Thread.castling_end-Thread.castling_start
		lea   rsi, [rbp-Thread.rootPos+Thread.castling_start]
		lea   rdi, [r13-Thread.rootPos+Thread.castling_start]
	  rep movsb

	; copy basic position info
		lea   rsi, [rbp]
		lea   rdi, [r13]
		mov   ecx, Pos._copy_size/8
	  rep movsq

	; rsi = address of our state table
		mov   rsi, qword[rbp+Pos.stateTable]

	; r14 = size of states that need to be copied
		mov   r14, qword[rbp+Pos.state]
		sub   r14, rsi
		add   r14, sizeof.State

	; if destination has no table, we need to alloc
		mov   rdi, qword[r13+Pos.stateTable]
	       test   rdi, rdi
		 jz   .alloc

	; rdx = capacity of states in destination
		mov   rdx, qword[r13+Pos.stateEnd]
		sub   rdx, rdi

	; if rdx < r14, we need to realloc
		cmp   rdx, r14
		 jb   .realloc
*/
Position_CopyTo.copy_states:
/*
	; copy State elements
		mov   rcx, r14
		shr   ecx, 3
	  rep movsq

	; set pointer to destination state
		sub   rdi, sizeof.State
		mov   qword[r13+Pos.state], rdi

		pop   r14 r13 rdi rsi rbx
		ret
*/
Position_CopyTo.realloc:
/*
		mov   rcx, rdi
		; rdx already has the size
	       call   _VirtualFree
*/
Position_CopyTo.alloc:
/*
		lea   rcx, [2*r14]
	       call   _VirtualAlloc
		mov   rdi, rax
		mov   qword[r13+Pos.stateTable], rax
		lea   rax, [rax+2*r14]
		mov   qword[r13+Pos.stateEnd], rax
		jmp   .copy_states
*/


Position_CopyToSearch:
/*
	; rbp address of source position
	; rcx address of destination position
	;  up to Pos._copy_size is copied
	;  and state array is copied
	; enough elements are copied for
	;   draw by 50 move detection
	;   referencing ss-5 and ss+2 in search

	       push   rsi rdi r13
		mov   r13, rcx

	; copy castling data
		mov   ecx, Thread.castling_end-Thread.castling_start
		lea   rsi, [rbp-Thread.rootPos+Thread.castling_start]
		lea   rdi, [r13-Thread.rootPos+Thread.castling_start]
	  rep movsb

	; copy basic position info
		lea   rsi, [rbp]
		lea   rdi, [r13]
		mov   ecx, Pos._copy_size/8
	  rep movsq

	; if destination has no table, we need to alloc
		mov   r9, qword[r13+Pos.stateTable]
	       test   r9, r9
		 jz   .alloc

	; rcx = capacity of states in destination
	; if rcx < MAX_PLY+102, we need to realloc
		mov   rdx, qword[r13+Pos.stateEnd]
		sub   rdx, r9
		cmp   rdx, sizeof.State*(100+MAX_PLY+2)
		 jb   .realloc
*/
Position_CopyToSearch.copy_states:
/*	; r9 = address of its state table
	; r8 = address of our state table
		mov   r8, qword[rbp+Pos.stateTable]

		mov   r10, qword[rbp+Pos.state]
		lea   r11, [r9+100*sizeof.State]
		mov   qword[r13+Pos.state], r11
*/
Position_CopyToSearch.loop:
/*		mov   rsi, r10
		mov   rdi, r11
		mov   ecx, sizeof.State/8
	  rep movsq
	; make sure that pliesFromNull never references a state past the beginning
	;  we don't want to fall of the cliff when checking 50 move rule
		mov   edx, 100
	      movzx   eax, word[r11+State.pliesFromNull]
		cmp   eax, edx
	      cmova   eax, edx
		mov   word[r11+State.pliesFromNull], ax

		sub   r10, sizeof.State
		sub   r11, sizeof.State
		cmp   r11, r9
		 jb   .done
		cmp   r10, r8
		jae   .loop
*/
Position_CopyToSearch.done:
/*		pop   r13 rdi rsi
		ret

*/
Position_CopyToSearch.realloc:
/*		mov   rcx, r9
		; rdx already has the size
	       call   _VirtualFree
*/
Position_CopyToSearch.alloc:
/*		mov   ecx, sizeof.State*(100+MAX_PLY+2)
	       call   _VirtualAlloc
		mov   r9, rax
		mov   qword[r13+Pos.stateTable], rax
		lea   rax, [rax+sizeof.State*(100+MAX_PLY+2)]
		mov   qword[r13+Pos.stateEnd], rax
		jmp   .copy_states
*/



Position_SetExtraCapacity:
/*	; in: rbp postion
	; reserve space for at least ecx states past the current

               imul   ecx, sizeof.State
                add   rcx, qword[rbp+Pos.state]
                cmp   rcx, qword[rbp+Pos.stateEnd]
		jae   .realloc
		ret
*/
Position_SetExtraCapacity.realloc:
/*	       push   rbx rsi rdi
		sub   rcx, qword[rbp+Pos.stateTable]
                mov   eax, ecx
                xor   edx, edx
                mov   ecx, sizeof.State
                div   ecx
                lea   ecx, [rax+8]
                shr   ecx, 2
                add   ecx, eax
               imul   ecx, sizeof.State
		mov   ebx, ecx
	       call   _VirtualAlloc
		mov   r8, rax
		lea   r10, [rax+rbx]
		mov   rsi, qword[rbp+Pos.stateTable]
		mov   rdi, r8
		mov   r9, qword[rbp+Pos.state]
		sub   r9, rsi
		add   r9, r8
		mov   rcx, qword[rbp+Pos.stateEnd]
		sub   rcx, rsi
		shr   ecx, 3
	  rep movsq
		mov   rcx, qword[rbp+Pos.stateTable]
		mov   rdx, qword[rbp+Pos.stateEnd]
		sub   rdx, rcx
		mov   rbx, r9
		mov   qword[rbp+Pos.state], r9
		mov   qword[rbp+Pos.stateTable], r8
		mov   qword[rbp+Pos.stateEnd], r10
	       call   _VirtualFree
		pop   rdi rsi rbx
		ret
*/


PrintBitboardCompact:
/*	       push   rsi
		mov   rsi, rcx
	@@:    test   rsi, rsi
		 jz   @f
		bsf   rcx, rsi
	       blsr   rsi, rsi, rax
	       call   PrintSquare
		mov   al, ' '
	      stosb
		jmp   @b
	@@:	pop   rsi
		ret
*/
        stp  x26, x30, [sp, -16]!
        mov  x26, x1
        mov  x0, x26
         bl  PrintHex 
        mov  w0, ' '
       strb  w0, [x27], 1
PrintBitboardCompact.Loop:
        cbz  x26, PrintBitboardCompact.Done
       rbit  x1, x26
        clz  x1, x1
         bl  PrintSquare
        mov  w0, ' '
       strb  w0, [x27], 1
        sub  x4, x26, 1
        and  x26, x26, x4
          b  PrintBitboardCompact.Loop
PrintBitboardCompact.Done:
        ldp  x26, x30, [sp], 16
        ret


Position_Print:
        stp  x21, x30, [sp, -16]!
        ldr  x21, [x20, Pos.state]
         bl  Position_PrintFen
        PrintNewLine

        adr  x1, Position_Print.sz_white
         bl  PrintString
        ldr  x1, [x20, 8*White]
         bl  PrintBitboardCompact
        PrintNewLine
        adr  x1, Position_Print.sz_black
         bl  PrintString
        ldr  x1, [x20, 8*Black]
         bl  PrintBitboardCompact
        PrintNewLine
        adr  x1, Position_Print.sz_pawn
         bl  PrintString
        ldr  x1, [x20, 8*Pawn]
         bl  PrintBitboardCompact
        PrintNewLine
        adr  x1, Position_Print.sz_knight
         bl  PrintString
        ldr  x1, [x20, 8*Knight]
         bl  PrintBitboardCompact
        PrintNewLine
        adr  x1, Position_Print.sz_bishop
         bl  PrintString
        ldr  x1, [x20, 8*Bishop]
         bl  PrintBitboardCompact
        PrintNewLine
        adr  x1, Position_Print.sz_rook
         bl  PrintString
        ldr  x1, [x20, 8*Rook]
         bl  PrintBitboardCompact
        PrintNewLine
        adr  x1, Position_Print.sz_queen
         bl  PrintString
        ldr  x1, [x20, 8*Queen]
         bl  PrintBitboardCompact
        PrintNewLine
        adr  x1, Position_Print.sz_king
         bl  PrintString
        ldr  x1, [x20, 8*King]
         bl  PrintBitboardCompact
        PrintNewLine

        ldp  x21, x30, [sp], 16
        ret
Position_Print.sz_white:  .ascii "white:     \0"
Position_Print.sz_black:  .ascii "black:     \0"
Position_Print.sz_pawn:   .ascii "pawn:      \0"
Position_Print.sz_knight: .ascii "knight:    \0"
Position_Print.sz_bishop: .ascii "bishop:    \0"
Position_Print.sz_rook:   .ascii "rook:      \0"
Position_Print.sz_queen:  .ascii "queen:     \0"
Position_Print.sz_king:   .ascii "king:      \0"

