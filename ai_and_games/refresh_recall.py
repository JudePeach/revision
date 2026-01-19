recall_notes = open("revision_notes.txt", "r")
refreshed_file = open("recall_notes_refresher.txt", "a")

for line in recall_notes:
    if not line.startswith("\t"):
        refreshed_file.write(line)

recall_notes.close()
refreshed_file.close()