"""
Tic Tac Toe Player
"""

from copy import deepcopy
from random import choice
from math import inf

X = "X"
O = "O"
EMPTY = None


def flatten(l):
    return [n for row in l for n in row]


def initial_state():
    """
    Returns starting state of the board.
    """
    return [[EMPTY, EMPTY, EMPTY],
            [EMPTY, EMPTY, EMPTY],
            [EMPTY, EMPTY, EMPTY]]


def player(board):
    """
    Returns player who has the next turn on a board.
    """
    # Flatten the board
    board = flatten(board)

    # X starts if board is empty
    if not any(board):
        return X

    # If more Xs than Os, it is O's turn
    if board.count(X) > board.count(O):
        return O
    # Otherwise, it must be X's turn (or end of game)
    else:
        return X


def actions(board):
    """
    Returns set of all possible actions (i, j) available on the board.
    """
    actions = set()
    for i, row in enumerate(board):
        for j, cell in enumerate(row):
            if cell is None:
                actions.add((i, j))
    return actions


def result(board, action):
    """
    Returns the board that results from making move (i, j) on the board.
    """
    copy = deepcopy(board)
    i, j = action

    # Check for valid move
    if action not in actions(copy):
        raise ValueError("Not a valid move")

    # Update the coorisponding cell with the correct player token
    copy[i][j] = player(copy)
    return copy


def winner(board):
    """
    Returns the winner of the game, if there is one.
    """
    wins = set()

    # Replace None (empty spaces) with some wildcard
    modified = [[n if n is not None else '*' for n in row] for row in board]

    # Get horizontal patterns
    for row in modified:
        wins.add(''.join(row))

    # Get vertical patterns
    for i in range(3):
        wins.add(''.join([modified[j][i] for j in range(3)]))

    # get primary diagonal
    wins.add(''.join([modified[i][i] for i in range(3)]))

    # get secondary diagonal
    wins.add(''.join([modified[2-i][i] for i in range(3)]))

    # Check for winning patterns or tie/game imcomplete
    if 'XXX' in wins:
        return X
    elif 'OOO' in wins:
        return O
    else:
        return None


def terminal(board):
    """
    Returns True if game is over, False otherwise.
    """
    # Game is over if all cells are filled or someone won early
    return all(flatten(board)) or winner(board)


def utility(board):
    """
    Returns 1 if X has won the game, -1 if O has won, 0 otherwise.
    """
    winplayer = winner(board)

    if winplayer == X:
        return 1
    elif winplayer == O:
        return -1
    else:  # It's a tie
        return 0


def minimax(board):
    """
    Returns the optimal action for the current player on the board.
    """
    # If the game was finished for some reason
    if terminal(board):
        return None

    # In case of empty board (AI takes first move), pick a random move (to save time)
    if not any(flatten(board)):
        return choice(tuple(actions(board)))

    _, best_play = alphabeta(board, player(board), -inf, inf)
    # _, best_play = minimax_basic(board, player(board))

    return best_play


def minimax_basic(board, player):
    """
    Basic Minimax implementation with no optimizations
    """
    # End condition
    if terminal(board):
        return utility(board), None

    # Maximizing player
    if player == X:
        v = -inf
        for action in actions(board):
            k, _ = minimax_basic(result(board, action), O)
            if k > v:
                v = k
                best_play = action
    # Minimizing player
    else:
        v = inf
        for action in actions(board):
            k, _ = minimax_basic(result(board, action), X)
            if k < v:
                v = k
                best_play = action

    return v, best_play


def alphabeta(board, player, alpha, beta):
    """
    Minimax implementation with alpha-beta pruning
    """
    # End condiction
    if terminal(board):
        return utility(board), None

    # Maximizing player
    if player == X:
        v = -inf
        for action in actions(board):
            k, _ = alphabeta(result(board, action), O, alpha, beta)
            if k > v:
                v = k
                best_play = action
            alpha = max(alpha, v)
            if alpha >= beta:
                break

    # Minimizing player
    else:
        v = inf
        for action in actions(board):
            k, _ = alphabeta(result(board, action), X, alpha, beta)
            if k < v:
                v = k
                best_play = action
            beta = min(beta, v)
            if alpha >= beta:
                break

    return v, best_play
