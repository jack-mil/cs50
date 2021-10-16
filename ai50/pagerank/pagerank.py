import os
import random
import re
import sys
from copy import copy

import numpy as np

DAMPING = 0.85
SAMPLES = 10000


def main():
    if len(sys.argv) != 2:
        sys.exit("Usage: python pagerank.py corpus")
    corpus = crawl(sys.argv[1])
    ranks = sample_pagerank(corpus, DAMPING, SAMPLES)
    print(f"PageRank Results from Sampling (n = {SAMPLES})")
    for page in sorted(ranks):
        print(f"  {page}: {ranks[page]:.4f}")
    ranks = iterate_pagerank(corpus, DAMPING)
    print(f"PageRank Results from Iteration")
    for page in sorted(ranks):
        print(f"  {page}: {ranks[page]:.4f}")


def crawl(directory):
    """
    Parse a directory of HTML pages and check for links to other pages.
    Return a dictionary where each key is a page, and values are
    a list of all other pages in the corpus that are linked to by the page.
    """
    pages = dict()

    # Extract all links from HTML files
    for filename in os.listdir(directory):
        if not filename.endswith(".html"):
            continue
        with open(os.path.join(directory, filename)) as f:
            contents = f.read()
            links = re.findall(r"<a\s+(?:[^>]*?)href=\"([^\"]*)\"", contents)
            pages[filename] = set(links) - {filename}

    # Only include links to other pages in the corpus
    for filename in pages:
        pages[filename] = set(
            link for link in pages[filename]
            if link in pages
        )

    return pages


def transition_model(corpus, page, damping_factor):
    """
    Return a probability distribution over which page to visit next,
    given a current page.

    With probability `damping_factor`, choose a link at random
    linked to by `page`. With probability `1 - damping_factor`, choose
    a link at random chosen from all pages in the corpus.

        corpus = {"1.html": {"2.html", "3.html"},
                  "2.html": {"3.html"},
                  "3.html": {"2.html"}}

        page = "1.html"

        damping = 0.85

        return = {"1.html": 0.05, "2.html": 0.475, "3.html": 0.475}
    """

    # If a page has no outgoing links,
    # operate as if it had links to *all* pages
    # i.e., pick from all pages equally
    if not len(corpus[page]):
        corpus = copy(corpus)  # copy mutable dictionary
        corpus[page] = set(corpus.keys())

    # Chance to visit any page in corpus
    all_chance = (1 - damping_factor) / len(corpus)
    # Chance to visit each linked page
    link_chance = damping_factor / len(corpus[page])

    # Set up dictionary to hold results. Weight 0
    distribution = {page: 0 for page in corpus}

    # All pages have weight `all_chance`
    # Add link bonus for links in current `page`
    for link in corpus:
        if link in corpus[page]:
            distribution[link] += link_chance
        distribution[link] += all_chance

    return distribution


def sample_pagerank(corpus, damping_factor, n):
    """
    Return PageRank values for each page by sampling `n` pages
    according to transition model, starting with a page at random.

    Return a dictionary where keys are page names, and values are
    their estimated PageRank value (a value between 0 and 1). All
    PageRank values should sum to 1.
    """

    # Begin with rank 0 for all pages
    page_rank = {page: 0 for page in corpus}

    # Initial sample is a random page
    current_page = random.choice(list(corpus.keys()))
    page_rank[current_page] += 1

    # For the remaining n-1 samples
    for i in range(n-1):
        # Calculate the transition model of the current page
        trans = transition_model(corpus, current_page, damping_factor)

        # Pick a new page based on the calculated probability weights
        current_page = np.random.choice(
            list(trans.keys()), 1, p=list(trans.values()))[0]

        # Count the number of hits per page
        page_rank[current_page] += 1

    return {k: float(v)/n for (k, v) in page_rank.items()}


def check_converge(old_rank, new_rank):

    values = zip(old_rank.values(), new_rank.values())
    return all([abs(o - n) < 0.001 for o, n in values])


def iterate_pagerank(corpus, damping_factor):
    """
    Return PageRank values for each page by iteratively updating
    PageRank values until convergence.

    Return a dictionary where keys are page names, and values are
    their estimated PageRank value (a value between 0 and 1). All
    PageRank values should sum to 1.
    """

    # Preprocess corpus to set pages with no links
    # as if linking to all pages
    corpus = copy(corpus)  # copy mutable dictionary
    for page in corpus:
        if not len(corpus[page]):
            corpus[page] = set(corpus.keys())

    # (1 - d) / N doesn't change
    const = (1-damping_factor) / len(corpus)

    # Initial ranking starts as 1/N for all pages
    current_rank = {page: 1/len(corpus) for page in corpus}

    # Iterate until convergence
    while True:
        new_rank = {}
        for page in corpus:

            link_ranks = []
            # For every page `i` in the corpus,
            # check if it links to the current page.
            # link_ranks is PR(i) / NumLinks(i) for each page `i`
            for i in corpus:
                if page in corpus[i]:
                    num_links = len(corpus[i])
                    link_ranks.append(current_rank[i] / num_links)

            # Calculate new rank based on formula
            # PR(p) = (1 - d) / N + d*Σ_i(PR(i) / NumLinks(i))
            new_rank[page] = const + damping_factor * sum(link_ranks)

        # If no value has changed by more than 0.001, ranks have converged
        if(check_converge(current_rank, new_rank)):
            break

        current_rank = new_rank

    return current_rank


if __name__ == "__main__":
    main()
