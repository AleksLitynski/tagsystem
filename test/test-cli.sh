BASEDIR=$(dirname "$0")

# cleanup db
export TSDBPATH="$BASEDIR/test_db"
echo "tsdbpath: $TSDBPATH"
echo ""
rm -r $TSDBPATH 2> /dev/null

# get reference to exe
TSYS="$BASEDIR/../cli/tsys"

# changeset/presentworkingset examples 
$TSYS pws

$TSYS cs +a+a
$TSYS pws

$TSYS cs ~a+zebra
$TSYS pws

$TSYS cs +bats
$TSYS pws

$TSYS cs ~~bear

$TSYS cs --silent ~~cart
$TSYS pws

$TSYS cs +aa +b
$TSYS cs +b
$TSYS cs +a
$TSYS cs +zebra
$TSYS cs +cart
$TSYS cs +a
$TSYS cs +zebra


# make/remove samples
# tag/untag samples
# list samples
echo "made toad doc"
$TSYS mk ~~toad
$TSYS pws
echo "added rat tag, removed toad tag"
$TSYS ls | $TSYS tag rat ~toad
$TSYS pws
echo "listed all"
$TSYS ls
$TSYS pws
echo "removed toad, added rat"
$TSYS cs ~toad+rat
$TSYS pws
echo "listed all"
$TSYS ls


# test all flags
$TSYS ls -i
$TSYS ls --id
$TSYS ls -i --tags
$TSYS ls -it

$TSYS mk --id --preview bear
$TSYS mk --id -ps squid

# $TSYS rm --id --force --silent ...

# $TSYS tag --id --force --silent ...
# $TSYS cs --silent ...
# $TSYS pws
