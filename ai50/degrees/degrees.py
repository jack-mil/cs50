import csv
import sys
from util import Node, StackFrontier, QueueFrontier
import pickle
from timeit import default_timer as time
import argparse as argp

# Maps names to a set of corresponding person_ids
names = {}

# Maps person_ids to a dictionary of: name, birth, movies (a set of movie_ids)
people = {}

# Maps movie_ids to a dictionary of: title, year, stars (a set of person_ids)
movies = {}


def main():

    parser = argp.ArgumentParser(
        description='Search IMDB database for connection between two characters')
    parser.add_argument('-d', '--directory', default='large', required=False,
                        help='directory where csv databases are located')
    parser.add_argument('-t', '--timeout', required=False,
                        default=60, help='search timeout', type=int)
    args = parser.parse_args()

    # Load data from files into memory
    print("Loading data...")
    load_data(args.directory)
    print("Data loaded.")

    print("Welcome. When done searching, a keyboard interrupt will exit the program")
    # Loop until exited
    try:
        while True:
            try:
                source = person_id_for_name(input("Source Name: "))
                target = person_id_for_name(input("Target Name: "))
            except ValueError:
                print("Person not found")
                continue

            try:
                path = shortest_path(source, target, args.timeout)
            except KeyboardInterrupt:
                print("Canceling search...")
                continue
            except TimeoutError:
                print("Timeout expired. Try reversing source and target")
                continue

            if path is None:
                print("Not connected")
            else:
                degrees = len(path)
                print(f"{degrees} degrees of separation.")
                path = [(None, source)] + path
                for i in range(degrees):
                    person1 = people[path[i][1]]["name"]
                    person2 = people[path[i + 1][1]]["name"]
                    movie = movies[path[i + 1][0]]["title"]
                    print(f"{i + 1}: {person1} and {person2} starred in {movie}")
    except KeyboardInterrupt:
        sys.exit("\nExited\n")


def load_data(directory):
    """
    Load data from CSV files into memory.
    """
    global names
    global people
    global movies

    cache = f"{directory}/dict.pickle"
    try:
        with open(cache, "rb") as dictfile:
            print("... from cache")
            names, people, movies = pickle.load(dictfile)
    except FileNotFoundError:
        print("... from csv")
        # Load people
        with open(f"{directory}/people.csv", encoding="utf-8") as f:
            reader = csv.DictReader(f)
            for row in reader:
                people[row["id"]] = {
                    "name": row["name"],
                    "birth": row["birth"],
                    "movies": set()
                }
                if row["name"].lower() not in names:
                    names[row["name"].lower()] = {row["id"]}
                else:
                    names[row["name"].lower()].add(row["id"])

        # Load movies
        with open(f"{directory}/movies.csv", encoding="utf-8") as f:
            reader = csv.DictReader(f)
            for row in reader:
                movies[row["id"]] = {
                    "title": row["title"],
                    "year": row["year"],
                    "stars": set()
                }

        # Load stars
        with open(f"{directory}/stars.csv", encoding="utf-8") as f:
            reader = csv.DictReader(f)
            for row in reader:
                try:
                    people[row["person_id"]]["movies"].add(row["movie_id"])
                    movies[row["movie_id"]]["stars"].add(row["person_id"])
                except KeyError:
                    pass

        # Pickle the dictionaries
        with open(cache, "wb") as dumpfile:
            pickle.dump((names, people, movies), dumpfile)


def shortest_path(source, target, timeout):
    """
    Returns the shortest list of (movie_id, person_id) pairs
    that connect the source to the target.

    If no possible path, returns None.

    Nodes will consist of a state (person_id) and an action (movie_id).
    A node will also have a parent, the node that generated the child node
    """
    if source == target:
        return None

    # Setup data structures
    frontier = QueueFrontier()
    explored = set()

    # The root node
    frontier.add(Node(state=source, parent=None, action=None))

    start = time()
    while True:
        if (time() - start) > timeout:
            raise TimeoutError
        # If nothing is in the frontier, there is no path
        if frontier.empty():
            return None

        # Get a node from the queue frontier
        node = frontier.remove()

        # Add unique (movie, person pair) to the explored set
        explored.add((node.action, node.state))

        # Add neighbors to the frontier
        for movie, person in neighbors_for_person(node.state):
            if (movie, person) not in explored:
                # Create a new child node for each neighbor
                child = Node(state=person, parent=node, action=movie)

                # Check for goal state and generate shortest path
                if child.state == target:
                    path = []
                    while child.parent is not None:
                        path.append((child.action, child.state))
                        child = child.parent
                    return path[::-1]
                frontier.add(child)


def person_id_for_name(name):
    """
    Returns the IMDB id for a person's name,
    resolving ambiguities as needed.
    """
    person_ids = list(names.get(name.lower().strip(), set()))
    if len(person_ids) == 0:
        raise ValueError
    elif len(person_ids) > 1:
        print(f"Which '{name}'?")
        for person_id in person_ids:
            person = people[person_id]
            name = person["name"]
            birth = person["birth"]
            print(f"ID: {person_id}, Name: {name}, Birth: {birth}")

        person_id = input("Intended Person ID: ")
        if person_id in person_ids:
            return person_id
        else:
            raise ValueError
    else:
        return person_ids[0]


def neighbors_for_person(person_id):
    """
    Returns (movie_id, person_id) pairs for people
    who starred with a given person.
    """
    movie_ids = people[person_id]["movies"]
    neighbors = set()
    for movie_id in movie_ids:
        for person_id in movies[movie_id]["stars"]:
            neighbors.add((movie_id, person_id))
    return neighbors


if __name__ == "__main__":
    main()
