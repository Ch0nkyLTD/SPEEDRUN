from speedrun.build_app import init_db, db
from speedrun.app import app


with app.app_context():
    init_db()
