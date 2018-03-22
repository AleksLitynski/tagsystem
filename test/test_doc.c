#include "test.h"

char * read_file(char * filename) {
    char * buffer = 0;
    long length;
    FILE * f = fopen (filename, "rb");

    if (f)
    {
        fseek (f, 0, SEEK_END);
        length = ftell (f);
        fseek (f, 0, SEEK_SET);
        buffer = malloc (length+1);
        if (buffer)
        {
            fread (buffer, 1, length, f);
        }
        fclose (f);
    }
    buffer[length] = '\0';

    return buffer;
}

void doc_test(void ** state) {
    test_state * st = (test_state*)*state;

    char * contents, * read;
    contents = "some text";
    ts_doc doc, doc2;
    ts_id doc_id;

    // create a document and write some text to it
    ts_doc_create(&doc, st->db);
    ts_id_dup(&doc.id, &doc_id);
    fs_write (doc.path, contents);
    // close the document
    ts_doc_close(&doc);

    // reopen the document and read the text
    ts_doc_open(&doc2, st->db, doc_id);
    read = read_file(doc2.path);
    ts_doc_delete(&doc2);

    // confirm the index was deleted
    sds idx = sdsnew("index");
    sds val = ts_id_string(&doc_id, sdsempty());
    bool delete_success = ts_db_test(st->db, idx, val) == TS_KEY_NOT_FOUND;
    sdsfree(idx);
    sdsfree(val);

    // confirm the document had the correct text in it
    assert_string_equal(contents, read);
    assert_true(delete_success);
    free(read);
}

