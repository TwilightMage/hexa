#include "TileDemoDomainEntity.h"

void TileDemoDomainEntity::on_start()
{
    make_body_static();
}

bool TileDemoDomainEntity::is_rigid_body()
{
    return true;
}
