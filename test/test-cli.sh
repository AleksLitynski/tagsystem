BASEDIR=$(dirname "$0")

# cleanup db
export TSDBPATH="$BASEDIR/test_db"
echo "tsdbpath: $TSDBPATH"
echo ""
rm -r $TSDBPATH 2> /dev/null

# get reference to exe
TSYS="$BASEDIR/../cli/tsys"

# changeset/presentworkingset examples 
# $TSYS pws

# $TSYS cs +a+a
# $TSYS pws

# $TSYS cs ~a+zebra
# $TSYS pws

# $TSYS cs +bats
# $TSYS pws

# $TSYS cs ~~bear

# $TSYS cs --silent ~~cart
# $TSYS pws

$TSYS cs +aa +b
$TSYS cs +b
# $TSYS cs +a
# $TSYS cs +zebra
# $TSYS cs +cart
# $TSYS cs +a
# $TSYS cs +zebra


# make/remove samples


# tag/untag samples


# list samples

