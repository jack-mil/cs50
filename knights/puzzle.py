from logic import *

AKnight = Symbol("A is a Knight")
AKnave = Symbol("A is a Knave")

BKnight = Symbol("B is a Knight")
BKnave = Symbol("B is a Knave")

CKnight = Symbol("C is a Knight")
CKnave = Symbol("C is a Knave")

# Exclusive Or helper function
def Xor(a, b):
    return (Or(a, b), Not(And(a, b)))

# Puzzle 0
# A says "I am both a knight and a knave."
knowledge0 = And(
    *Xor(AKnight, AKnave), # General Knowledge

    Biconditional(AKnight, And(AKnight, AKnave)) # If what A said is true, A must be a knight, and vice versa
    )

# Puzzle 1
# A says "We are both knaves."
# B says nothing.
knowledge1 = And(
    *Xor(AKnight, AKnave), # General Knowledge
    *Xor(BKnight, BKnave),

    Biconditional(AKnight, And(AKnave, BKnave)) # If what A said is true, A must be a knight, and vice versa

)

# Puzzle 2
# A says "We are the same kind."
# B says "We are of different kinds."
knowledge2 = And(
    *Xor(AKnight, AKnave), # General Knowledge
    *Xor(BKnight, BKnave),

    # These statements are much more complicated than they need to be, but I tried to encode them without including any logic in my head
    Biconditional(AKnight, Or(And(AKnight, BKnight), And(AKnave, BKnave))), # If A is a knight, A and B are the same type
    Biconditional(BKnight, Or(And(AKnight, BKnave), And(BKnight, AKnave))) # If B is a knight, A and B are different types
)

# Puzzle 3
# A says either "I am a knight." or "I am a knave.", but you don't know which.
# B says "A said 'I am a knave'."
# B says "C is a knave."
# C says "A is a knight."
knowledge3 = And(
    *Xor(AKnight, AKnave), # General Knowledge
    *Xor(BKnight, BKnave),
    *Xor(CKnight, CKnave),


    Implication(BKnight, Biconditional(AKnight, AKnave)), # If B were to a be a knight, then A must really have said "I am a knave". 
                                                        # If A were a Knight, that statement would be true, and if a knave, That statment false
    Biconditional(BKnight, CKnave), # If B is a knight, then C is a knave, and vice versa

    Biconditional(CKnight, AKnight) # If C is a knight, then A is also a knight, and vice versa
)



def main():
    symbols = [AKnight, AKnave, BKnight, BKnave, CKnight, CKnave]
    puzzles = [
        ("Puzzle 0", knowledge0),
        ("Puzzle 1", knowledge1),
        ("Puzzle 2", knowledge2),
        ("Puzzle 3", knowledge3)
    ]
    for puzzle, knowledge in puzzles:
        print(puzzle)
        if len(knowledge.conjuncts) == 0:
            print("    Not yet implemented.")
        else:
            for symbol in symbols:
                if model_check(knowledge, symbol):
                    print(f"    {symbol}")


if __name__ == "__main__":
    main()
