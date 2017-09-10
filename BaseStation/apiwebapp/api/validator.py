import json


class Validator:
    def __init__(self, input, rules):
        self.errors = []

        for rule in rules:
            if rule not in input or not input[rule]:
                self.errors.append("'" + rule + "' must be provided and have a value")

    def has_errors(self):
        return self.errors

    def get_errors(self):
        return self.errors

    def get_message(self):
        return json.dumps({'success': False, 'errors': self.get_errors()})
