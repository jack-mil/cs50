class Node():
    def __init__(self, state, parent, action):
        self.state = state
        self.parent = parent
        self.action = action


class StackFrontier():
    """A frontier structure implmented as a stack (LIFO)"""
    def __init__(self):
        self.frontier = []

    def add(self, node):
        """Adds a node to the end of the frontier"""
        self.frontier.append(node)

    def contains_state(self, state):
        """Returns true if state is found in the frontier"""
        return any(node.state == state for node in self.frontier)

    def empty(self):
        """Returns true if the frontier is empty"""
        return len(self.frontier) == 0

    def remove(self):
        """Returns the node last added to the frontier' (LIFO)"""
        if self.empty():
            raise Exception("empty frontier")
        else:
            node = self.frontier[-1]
            self.frontier = self.frontier[:-1]
            return node


class QueueFrontier(StackFrontier):
    """A frontier structure implmented as a queue (FIFO)"""

    def remove(self):
        """Returns first node in the frontier (FIFO)"""
        if self.empty():
            raise Exception("empty frontier")
        else:
            node = self.frontier[0]
            self.frontier = self.frontier[1:]
            return node
