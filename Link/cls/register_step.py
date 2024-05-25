import time

from .property import Props
from .create_step import CreateStep


class register_step:
    def __init__(self):
        """
        Wrapper class to implement before & after methods for CreateStep
        :param args: *
        :param kwargs: Available [Debug: bool, Persist: bool]
        Debug - logs processing time for a Step
        Persist - saves Step returns in Props
        """
        self.persist = None
        self.debug = None

    def __call__(self, func):
        def wrapper(name: str, params: dict, *args, **kwargs):
            result = None

            self.debug = kwargs.get('Debug')
            self.persist = kwargs.get('Persist')

            if self.before_execute(name):

                try:
                    result = func(name, params, **kwargs)
                except Exception as e:
                    if self.debug:
                        Props.get_logger().error(f"{str(e)}\n")

                self.after_execute(name, result)

            return result

        setattr(CreateStep, func.__name__, wrapper)
        return wrapper

    def before_execute(self, name) -> bool:
        """
        :return: Validates conditional skipping of Step
        """
        exec = False if Props.get_next() and Props.get_next() is not name else True
        if self.debug:
            self.start_time = time.time() * 1000
        return exec

    def after_execute(self, name, result):
        """
        End of Step exec, kills self
        """
        if self.debug:
            Props.get_logger().info(f'{name} - Process Time: {str(time.time() * 1000 - self.start_time)}')
        if self.persist:
            Props.set_results(name, result)
        del self