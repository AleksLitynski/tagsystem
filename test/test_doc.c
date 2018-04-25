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
        buffer = malloc(length+1);
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
    fs_write(doc.path, contents);
    ts_doc_close(&doc);

    // // reopen the document and read the text
    ts_doc_open(&doc2, st->db, doc_id);
    read = read_file(doc2.path);
    ts_doc_delete(&doc2);
    ts_doc_close(&doc2);

    // confirm the index was deleted
    sds val = ts_id_string(&doc_id, sdsempty());
    ts_db_begin_txn(st->db);
    bool delete_success = ts_db_test(st->db, "index", val) == TS_KEY_NOT_FOUND;
    ts_db_commit_txn(st->db);
    sdsfree(val);

    // confirm the document had the correct text in it
    assert_string_equal(contents, read);
    free(read);
    assert_true(delete_success);
}


void doc_unique_test(void ** state) {
    // generate 25 random docs and confirm no duplicates. Really robust test.
    test_state * st = (test_state*)*state;

    int items = 25;
    ts_id ids[items];
    
    for(int i = 0; i < items; i++) {
        ts_doc doc;
        ts_doc_create(&doc, st->db);
        ts_id_dup(&doc.id, &ids[i]);
        ts_doc_close(&doc);
    }
    
    int duplicates = 0;
    for(int i = 0; i < items; i++) {
        for(int j = 0; j < items; j++) {
            if(ts_id_eq(&ids[i], &ids[j]) && i != j) {
                duplicates++;
                LOG1("DUPLICATE:");
                LOGID(&ids[i]);
            }
        }
    }


    assert_int_equal(duplicates, 0);
}