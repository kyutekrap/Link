from .property import Props


class register_flow:
    def __init__(self):
        pass

    def __call__(self, func):
        def wrapper(*args, **kwargs):

            Props()

            return func(*args, **kwargs)
        return wrapper
