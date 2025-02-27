from speedrun.db import db
import os
from dataclasses import dataclass, asdict
import json
import time

# Types
# db.Column(db.Integer, primary_key=True)
# db.Column(db.String)
# interhit db.Model
# @TODO: wtf is a sql enum?
SESSION_OK = "alive"
SESSION_DEAD = "dead"
SESSION_INACTIVE = "inactive"

TASK_CREATED = "created"
TASK_STARTED = "started"
TASK_RUNNING = "running"
TASK_FAIL = "fail"
TASK_OK = "ok"


# TODO: maybe have separate table for builds? i.e. differnt implant builds


@dataclass
class Session(db.Model):
    id: int = db.Column(db.Integer, primary_key=True)
    status: str = db.Column(db.String, default=SESSION_OK)
    created_at: int = db.Column(db.Integer)
    last_seen: int = db.Column(db.Integer)
    os_info: str = db.Column(db.String)

    def toJSON(self):
        return asdict(self)


@dataclass
class Task(db.Model):
    id: int = db.Column(db.Integer, primary_key=True)
    # TODO: wtf is forieign ke?
    session_id: int = db.Column(db.Integer)
    status: str = db.Column(db.String, default=TASK_CREATED)
    creation_date: int = db.Column(db.Integer)
    finish_date: int = db.Column(db.Integer)
    # TODO: needs to be non null
    cmd: str = db.Column(db.String, nullable=False)
    args: str = db.Column(db.String)
    result: str = db.Column(db.String)

    def toJSON(self):
        return asdict(self)


def make_task(session_id: int, cmd: str, args: str):
    now = int(time.time())
    return Task(
        session_id=session_id,
        status=TASK_CREATED,
        creation_date=now,
        finish_date=0,
        cmd=cmd,
        args=args,
    )


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
