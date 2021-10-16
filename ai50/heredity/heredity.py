import csv
import itertools
import sys

PROBS = {

    # Unconditional probabilities for having gene
    "gene": {
        2: 0.01,
        1: 0.03,
        0: 0.96
    },

    "trait": {

        # Probability of trait given two copies of gene
        2: {
            True: 0.65,
            False: 0.35
        },

        # Probability of trait given one copy of gene
        1: {
            True: 0.56,
            False: 0.44
        },

        # Probability of trait given no gene
        0: {
            True: 0.01,
            False: 0.99
        }
    },

    # Mutation probability
    "mutation": 0.01
}


def main():

    # Check for proper usage
    if len(sys.argv) != 2:
        sys.exit("Usage: python heredity.py data.csv")
    people = load_data(sys.argv[1])

    # Keep track of gene and trait probabilities for each person
    probabilities = {
        person: {
            "gene": {
                2: 0,
                1: 0,
                0: 0
            },
            "trait": {
                True: 0,
                False: 0
            }
        }
        for person in people
    }

    # Loop over all sets of people who might have the trait
    names = set(people)
    for have_trait in powerset(names):

        # Check if current set of people violates known information
        fails_evidence = any(
            (people[person]["trait"] is not None and
             people[person]["trait"] != (person in have_trait))
            for person in names
        )
        if fails_evidence:
            continue

        # Loop over all sets of people who might have the gene
        for one_gene in powerset(names):
            for two_genes in powerset(names - one_gene):

                # Update probabilities with new joint probability
                p = joint_probability(people, one_gene, two_genes, have_trait)
                update(probabilities, one_gene, two_genes, have_trait, p)

    # Ensure probabilities sum to 1
    normalize(probabilities)

    # Print results
    for person in people:
        print(f"{person}:")
        for field in probabilities[person]:
            print(f"  {field.capitalize()}:")
            for value in probabilities[person][field]:
                p = probabilities[person][field][value]
                print(f"    {value}: {p:.4f}")


def load_data(filename):
    """
    Load gene and trait data from a file into a dictionary.
    File assumed to be a CSV containing fields name, mother, father, trait.
    mother, father must both be blank, or both be valid names in the CSV.
    trait should be 0 or 1 if trait is known, blank otherwise.
    """
    data = dict()
    with open(filename) as f:
        reader = csv.DictReader(f)
        for row in reader:
            name = row["name"]
            data[name] = {
                "name": name,
                "mother": row["mother"] or None,
                "father": row["father"] or None,
                "trait": (True if row["trait"] == "1" else
                          False if row["trait"] == "0" else None)
            }
    return data


def powerset(s):
    """
    Return a list of all possible subsets of set s.
    """
    s = list(s)
    return [
        set(s) for s in itertools.chain.from_iterable(
            itertools.combinations(s, r) for r in range(len(s) + 1)
        )
    ]


def joint_probability(people, one_gene, two_genes, have_trait):
    """
    Compute and return a joint probability.

    The probability returned should be the probability that
        * everyone in set `one_gene` has one copy of the gene, and
        * everyone in set `two_genes` has two copies of the gene, and
        * everyone not in `one_gene` or `two_gene` does not have the gene, and
        * everyone in set `have_trait` has the trait, and
        * everyone not in set` have_trait` does not have the trait.
    """
    prob = float(1)

    for person in people:
        # Establish some information (dict keys) for each person
        number_genes = count_genes(person, one_gene, two_genes)
        has_trait = person in have_trait

        mother = people[person]['mother']
        father = people[person]['father']

        # Every person has an unconditional probability of expressing a trait
        # given a number of genes
        prob *= PROBS['trait'][number_genes][has_trait]

        # Now calculate probabilities of genes existing

        # Check for parents and skip to next person
        if not any((mother, father)):
            prob *= PROBS['gene'][number_genes]
            continue

        # Otherwise calculate probablity based on parent's genes

        # Calculate parental transfer distribution
        chance = {mother: 0, father: 0}
        for parent in chance:
            chance[parent] = (
                # If the parent has two genes, there is 100% chance of
                # passing it on, unless mutation occurs
                1 - PROBS['mutation'] if parent in two_genes else

                # If the parent has one gene, 50% chance of passing
                0.5 if parent in one_gene else

                # Otherwise, child must get the gene by mutation
                PROBS['mutation']
            )

        prob *= (
            # % of getting 2 genes
            chance[mother] * chance[father] if number_genes == 2 else

            # % of getting 1 gene from mother and not father (& vice versa)
            chance[mother] * (1 - chance[father])
            + (1 - chance[mother]) * chance[father]
            if number_genes == 1 else

            # % of getting 0 genes
            (1 - chance[mother]) * (1 - chance[father])
        )
    # end for loop

    return prob


def update(probabilities, one_gene, two_genes, have_trait, p):
    """
    Add to `probabilities` a new joint probability `p`.
    Each person should have their "gene" and "trait" distributions updated.
    Which value for each distribution is updated depends on whether
    the person is in `have_gene` and `have_trait`, respectively.
    """

    for person in probabilities:

        # Update gene distribution for each person
        c = count_genes(person, one_gene, two_genes)
        probabilities[person]['gene'][c] += p

        # Update trait distribution depending on if person is in have_trait
        probabilities[person]['trait'][person in have_trait] += p


def normalize(probabilities):
    """
    Update `probabilities` such that each probability distribution
    is normalized (i.e., sums to 1, with relative proportions the same).
    """

    def norm(d):
        f = 1.0 / sum(d.values())
        return {key: value * f for key, value in d.items()}

    for person in probabilities:
        probabilities[person] = {
            key: norm(value)
            for key, value
            in probabilities[person].items()
        }


def count_genes(person, one_gene, two_genes):
    return (2 if person in two_genes else
            1 if person in one_gene else
            0)


if __name__ == "__main__":
    main()
