0 cat
0 _**
0 1 
1 rd
3  o





111111
000001








id_length
segment_size
file_size




255



each row has 3 fixed values, and one dynamic value:
    number of jumps
    id
    mask
    jump 1 - x

uniquely, number of jumps is stored at the top of the file, so:

2 <- number of items (only needed if you don't know how many items there are. Eg: where do the jump counts end and the items begin? Could also add \n after jump counts)
0 <- number of jumps the first item will have
1 <- number of jumps the second item will have
a <- first item
0 <- first item mask
b <- second item
1 <- second item mask
0 <- jump from second item's mask


to look up an item X item:

to delete an item:


could also store item inset? although I'm not totally sure it's worth it, spacewise.
so, item 2 is only 5 characters, instead of storing 250 0s, then 11111, we could store 250 inset, 2 jumps, 11111, 10010, (1, 2)
should provide a function to convert file from binary to ascii and visa versa. Should be able to operate on either file

(figure out the space math crap)

