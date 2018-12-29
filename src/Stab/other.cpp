 if (stabilisation_mode == 0) // made rate
        {
            // pid

            for (size_t i; i < 3; i++)
            {
                acro_commands[i] += (cmd[i] - ang[i]) * Kp_acro * dt; // error and integration
            }

            // send
            pid.getPID(pid_out, pid_debug, cmd, rates, dt);
        }
        else if (stabilisation_mode == 1)
        {
            for (size_t i; i < 3; i++)
            {
                acro_commands[i] = (cmd[i] - ang[i]) * Kp_acro; // error and integration
            }

            // send
            pid.getPID(pid_out, pid_debug, acro_commands, rates, dt);
        }
        else if (stabilisation_mode == 2)
        { // mode angle
            // pid
            pid.getPID(pid_out, pid_debug, cmd, ang, dt);
        }
