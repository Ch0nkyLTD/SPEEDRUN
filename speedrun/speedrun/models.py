from speedrun.db import db
import os
from dataclasses import dataclass, asdict
import json


# Types
# db.Column(db.Integer, primary_key=True)
# db.Column(db.String)
# interhit db.Model


@dataclass
class Foo(db.Model):
    id: int = db.Column(db.Integer, primary_key=True)
    foo: str = db.Column(db.String)
    bar: str = db.Column(db.String)

    def toJSON(self):
        return asdict(self)


def make_foo(foo: str, bar: str) -> Foo:
    # todo ensure the implant id exists!!
    # todo: use logggin instead of print!
    # todo: add validation!
    t = Foo(foo=foo, bar=bar)
    return t
