#include "SingleTile.h"

void SingleTile::on_start()
{
    make_body_static();
}

bool SingleTile::is_rigid_body()
{
    return true;
}
