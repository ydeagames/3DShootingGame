#pragma once

class PrimitiveModel
{
public:
	using generator_type = std::function<std::unique_ptr<DirectX::GeometricPrimitive>(ID3D11Device* device)>;

private:
	generator_type gen;
	std::unique_ptr<DirectX::GeometricPrimitive> model;

public:
	PrimitiveModel(const generator_type& gen)
		: gen(gen)
	{
	}

public:
	DirectX::GeometricPrimitive* GetOrCreate(ID3D11Device* device)
	{
		if (!model)
			model = gen(device);
		return model.get();
	}
};