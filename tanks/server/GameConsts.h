#pragma once

#include <utils/serialize.h>
#include <utils/fileutils.h>
#include <utils/xmlserializer.h>
#include <utils/memserializer.h>

struct CGameConsts
{
protected:
	const LPCTSTR m_szFileName;
	const LPCTSTR m_szRootXMLNode;
public:
	CGameConsts()
		:m_szFileName(_T("tankbattle.cfg"))
		,m_szRootXMLNode(_T("configuration"))
	{
		m_minpassability = 0.1;
#if defined(_DEBUG)
		m_width = 256;
		m_height = 256;
#else
		m_width = 512;
		m_height = 512;
#endif
		m_scale = 5.0;
		m_rockheight = 20.0;
		m_radarR = 96/2;
		m_fPH_g = 9.8066;
		m_fHorseStrength = 735.49875;
		m_fSphereRadius = 1.0;
		m_fAirResistanceCoef = 1.0;
		m_fInternalFrictionCoef = 10.0;
		m_fFrictionCoef = 1.0;
		m_fArmorBase = 1000.0;
		m_fArmorMass = 5000.0;
		m_fChassisMass = 2000;
		m_fMaxVelocity = 20;
		m_fChassisWidth = 3;
		m_fEnginePower = 1000;
		m_fEngineMass = 1000;
		m_fEngineFuel = 1000;
		m_fFuelConsumptionCoef = 0.1;
		m_fTankSphereRadius = 5.0;
		m_fShellMass = 15;
		m_fShellImpulseForcesModule = 14000;
		m_fShellKillability = 500;
		m_fShellKillability2 = 100;
		m_fShellSphere2 = 20;
		m_fShellSphereRadius = 4;
		m_fArmorArtefactCount = 1000;
		m_fArmorArtefactMass = 1000;
		m_fFuelArtefactMass = 100;

		//load();
	}
	~CGameConsts()
	{
		//save(m_szFileName);
	}

	double minpassability() const {return m_minpassability;}
	size_t width() const {return m_width;}
	size_t height() const {return m_height;}
	double scale() const {return m_scale;}
	double rockheight() const {return m_rockheight;}
	size_t radarR() const {return m_radarR;}
	double PH_g() const {return m_fPH_g;}
	double HorseStrength() const {return m_fHorseStrength;}
	double get_SphereRadius() const {return m_fSphereRadius;}
	double get_AirResistanceCoef() const {return m_fAirResistanceCoef;}
	double get_InternalFrictionCoef() const {return m_fInternalFrictionCoef;}
	double get_FrictionCoef() const {return m_fFrictionCoef;}
	double get_ArmorBase() const {return m_fArmorBase;}
	double get_ArmorMass() const {return m_fArmorMass;}
	double get_ChassisMass() const {return m_fChassisMass;}
	double get_MaxVelocity() const {return m_fMaxVelocity;}
	double get_ChassisWidth() const {return m_fChassisWidth;}
	double get_EnginePower() const {return m_fEnginePower;}
	double get_EngineMass() const {return m_fEngineMass;}
	double get_EngineFuel() const {return m_fEngineFuel;}
	double getFuelConsumptionCoef() const {return m_fFuelConsumptionCoef;}
	double get_TankSphereRadius() const {return m_fTankSphereRadius;}
	double get_ShellMass() const {return m_fShellMass;}
	double get_ShellImpulseForcesModule() const {return m_fShellImpulseForcesModule;}
	double get_ShellKillability() const {return m_fShellKillability;}
	double get_ShellKillability2() const {return m_fShellKillability2;}
	double get_ShellSphere2() const {return m_fShellSphere2;}
	double get_ShellSphereRadius() const {return m_fShellSphereRadius;}
	double get_ArmorArtefactCount() const {return m_fArmorArtefactCount;}
	double get_ArmorArtefactMass() const {return m_fArmorArtefactMass;} 
	double get_FuelArtefactMass() const {return m_fFuelArtefactMass;}
protected:
	double m_minpassability;
	size_t m_width;
	size_t m_height;
	double m_scale;
	double m_rockheight;
	size_t m_radarR;
	double m_fPH_g;
	double m_fHorseStrength;
	double m_fSphereRadius;
	double m_fAirResistanceCoef;
	double m_fInternalFrictionCoef;
	double m_fFrictionCoef;
	double m_fArmorBase;
	double m_fArmorMass;
	double m_fChassisMass;
	double m_fMaxVelocity;
	double m_fChassisWidth;
	double m_fEnginePower;
	double m_fEngineMass;
	double m_fEngineFuel;
	double m_fFuelConsumptionCoef;
	double m_fArtefactMass;
	double m_fTankSphereRadius;
	double m_fShellMass;
	double m_fShellImpulseForcesModule;
	double m_fShellKillability;
	double m_fShellKillability2;
	double m_fShellSphere2;
	double m_fShellSphereRadius;
	double m_fArmorArtefactCount;
	double m_fArmorArtefactMass;
	double m_fFuelArtefactMass;

public:
	struct DefaultLayout : public Layout<CGameConsts>
	{
		DefaultLayout()
		{
			add_simple(_T("MinimumPassability"),&CGameConsts::m_minpassability);
			add_simple(_T("Width"),&CGameConsts::m_width);
			add_simple(_T("Height"),&CGameConsts::m_height);
			add_simple(_T("Scale"),&CGameConsts::m_scale);
			add_simple(_T("RocHeight"),&CGameConsts::m_rockheight);
			add_simple(_T("RadarRadius"),&CGameConsts::m_radarR);
			add_simple(_T("G"),&CGameConsts::m_fPH_g);
			add_simple(_T("HorseStrength"),&CGameConsts::m_fHorseStrength);
			add_simple(_T("ShereRadius"),&CGameConsts::m_fSphereRadius);
			add_simple(_T("AirResistanceCoef"),&CGameConsts::m_fAirResistanceCoef);
			add_simple(_T("InternalFrictionCoef"),&CGameConsts::m_fInternalFrictionCoef);
			add_simple(_T("FrictionCoef"),&CGameConsts::m_fFrictionCoef);
			add_simple(_T("ArmorCount"),&CGameConsts::m_fArmorBase);
			add_simple(_T("ArmorMass"),&CGameConsts::m_fArmorMass);
			add_simple(_T("ChassisMass"),&CGameConsts::m_fChassisMass);
			add_simple(_T("MaxVelocity"),&CGameConsts::m_fMaxVelocity);
			add_simple(_T("ChassisWidth"),&CGameConsts::m_fChassisWidth);
			add_simple(_T("EnginePower"),&CGameConsts::m_fEnginePower);
			add_simple(_T("EngineMass"),&CGameConsts::m_fEngineMass);
			add_simple(_T("EngineFuel"),&CGameConsts::m_fEngineFuel);
			add_simple(_T("FuelConsumptionCoef"),&CGameConsts::m_fFuelConsumptionCoef);
			add_simple(_T("ArtefactMass"),&CGameConsts::m_fArtefactMass);
			add_simple(_T("TankSphereRadius"),&CGameConsts::m_fTankSphereRadius);
			add_simple(_T("ShellMass"),&CGameConsts::m_fShellMass);
			add_simple(_T("ShellImpulseForcesModule"),&CGameConsts::m_fShellImpulseForcesModule);
			add_simple(_T("ShellKillability"),&CGameConsts::m_fShellKillability);
			add_simple(_T("ShellKillability2"),&CGameConsts::m_fShellKillability2);
			add_simple(_T("ShellSphere2"),&CGameConsts::m_fShellSphere2);
			add_simple(_T("ShellSphereRadius"),&CGameConsts::m_fShellSphereRadius);
			add_simple(_T("ArmorArtefactCount"),&CGameConsts::m_fArmorArtefactCount);
			add_simple(_T("ArmorArtefactMass"),&CGameConsts::m_fArmorArtefactMass);
			add_simple(_T("FuelArtefactMass"),&CGameConsts::m_fFuelArtefactMass);
		}
	};

	void load()
	{
		CString sFileName = get_filename(m_szFileName);
		if(!FileUtils::IsFileExist(sFileName)) return;

		CXMLLoader xmlloader;
		xmlloader.loadFromFile(sFileName);

		CMemoryArchive arch1;
		arch1.visit(xmlloader);		// populate arch1

		::load(arch1,m_szRootXMLNode,*this,get_structlayout<CGameConsts>());
	}

	void save()
	{
		CString sFileName = get_filename(m_szFileName);

		CXMLSaver xmlsaver;
		CMemoryArchive archive;

		::save(archive,m_szRootXMLNode,*this,get_structlayout<CGameConsts>());
		archive.visit(xmlsaver);
		xmlsaver.write2File(sFileName,true);
	}

	CString get_filename(const CString& _sFileName)
	{
		CString sPath;
		FileUtils::GetModulePath(NULL,sPath);
		CString sFileName = sPath;
		FileUtils::SetLastSlash(sFileName);
		sFileName += _sFileName;
		return sFileName;
	}
};
