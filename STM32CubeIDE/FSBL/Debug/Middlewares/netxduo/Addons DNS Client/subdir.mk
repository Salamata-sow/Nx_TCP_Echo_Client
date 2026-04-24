################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/netxduo/Addons\ DNS\ Client/nxd_dns.c 

OBJS += \
./Middlewares/netxduo/Addons\ DNS\ Client/nxd_dns.o 

C_DEPS += \
./Middlewares/netxduo/Addons\ DNS\ Client/nxd_dns.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/netxduo/Addons\ DNS\ Client/nxd_dns.o: ../Middlewares/netxduo/Addons\ DNS\ Client/nxd_dns.c Middlewares/netxduo/Addons\ DNS\ Client/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32N657xx -DNX_INCLUDE_USER_DEFINE_FILE -DTX_INCLUDE_USER_DEFINE_FILE -DTX_SINGLE_MODE_SECURE=1 -c -I"C:/Users/PC/Downloads/Nx_TCP_Echo_Client/STM32CubeIDE/FSBL/Drivers/BSP/Components/lan8742" -I../../../FSBL/NetXDuo/App -I../../../FSBL/NetXDuo/Target -I../../../FSBL/Core/Inc -I../../../FSBL/AZURE_RTOS/App -I../../../Drivers/STM32N6xx_HAL_Driver/Inc -I../../../Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../../Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../../Drivers/BSP/Components/lan8742 -I../../../Middlewares/ST/netxduo/addons/dhcp -I../../../Middlewares/ST/netxduo/tsn/inc -I../../../Middlewares/ST/netxduo/common/drivers/ethernet -I../../../Middlewares/ST/threadx/common/inc -I../../../Middlewares/ST/netxduo/common/inc -I../../../Middlewares/ST/netxduo/ports/cortex_m55/gnu/inc -I../../../Middlewares/ST/threadx/ports/cortex_m55/gnu/inc -I../../../Drivers/CMSIS/Include -I"C:/Users/PC/Downloads/Nx_TCP_Echo_Client/Middlewares/ST/netxduo/addons/dns" -I"C:/Users/PC/Downloads/Nx_TCP_Echo_Client/Drivers/STM32N6xx_HAL_Driver" -I"C:/Users/PC/Downloads/Nx_TCP_Echo_Client/FSBL/Core/Src" -I"C:/Users/PC/Downloads/Nx_TCP_Echo_Client/FSBL/Core/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"Middlewares/netxduo/Addons DNS Client/nxd_dns.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-netxduo-2f-Addons-20-DNS-20-Client

clean-Middlewares-2f-netxduo-2f-Addons-20-DNS-20-Client:
	-$(RM) ./Middlewares/netxduo/Addons\ DNS\ Client/nxd_dns.cyclo ./Middlewares/netxduo/Addons\ DNS\ Client/nxd_dns.d ./Middlewares/netxduo/Addons\ DNS\ Client/nxd_dns.o ./Middlewares/netxduo/Addons\ DNS\ Client/nxd_dns.su

.PHONY: clean-Middlewares-2f-netxduo-2f-Addons-20-DNS-20-Client

