#include "codexion.h"

int try_take_dongles(t_coder *coder)
{
    long now;
    int left_dongle_ok;
    int right_dongle_ok;

    if (coder->left_dongle == coder->right_dongle)
        return (0);
    now = get_timestamp_ms();
    left_dongle_ok = (coder->left_dongle->in_use == 0)
                    && (now >= coder->left_dongle->table_return_time
                        + coder->config->dongle_cooldown);
    right_dongle_ok = (coder->right_dongle->in_use == 0)
                    && (now >= coder->right_dongle->table_return_time
                        + coder->config->dongle_cooldown);
    if (left_dongle_ok && right_dongle_ok)
    {
        coder->left_dongle->in_use = 1;
        log_state(coder->table, coder->id_coder, "has taken a dongle");
        coder->right_dongle->in_use = 1;
        log_state(coder->table, coder->id_coder, "has taken a dongle");
        return (1);
    }
    return (0);
}
