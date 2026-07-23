import duckdb

DB = "lahman-cmudb2025.db"

con = duckdb.connect(DB)

# -----------------------------
# columns
# -----------------------------
columns = con.execute("""
                      SELECT
                          table_name,
                          column_name,
                          data_type,
                          ordinal_position
                      FROM information_schema.columns
                      WHERE table_schema='main'
                      ORDER BY table_name, ordinal_position;
                      """).fetchall()

# -----------------------------
# primary key
# -----------------------------
pk = {}

try:
    rows = con.execute("""
                       SELECT
                           kcu.table_name,
                           kcu.column_name
                       FROM information_schema.table_constraints tc
                                JOIN information_schema.key_column_usage kcu
                                     ON tc.constraint_name = kcu.constraint_name
                       WHERE tc.constraint_type='PRIMARY KEY';
                       """).fetchall()

    for t,c in rows:
        pk.setdefault(t,set()).add(c)

except:
    pass

# -----------------------------
# foreign key
# -----------------------------
fks = []

try:
    rows = con.execute("""
                       SELECT
                           fk.table_name,
                           fk.column_name,
                           pk.table_name,
                           pk.column_name
                       FROM information_schema.referential_constraints rc

                                JOIN information_schema.key_column_usage fk
                                     ON rc.constraint_name=fk.constraint_name

                                JOIN information_schema.key_column_usage pk
                                     ON rc.unique_constraint_name=pk.constraint_name;
                       """).fetchall()

    fks.extend(rows)

except:
    pass

# -----------------------------
# output table
# -----------------------------

current = None

for table,col,tp,pos in columns:

    if table != current:

        if current is not None:
            print("}\n")

        print(f"Table {table} {{")
        current = table

    line = f"    {col} {tp.lower()}"

    if table in pk and col in pk[table]:
        line += " [pk]"

    print(line)

print("}")

print()

# -----------------------------
# output ref
# -----------------------------

for ft,fc,tt,tc in fks:
    print(f"Ref: {ft}.{fc} > {tt}.{tc}")